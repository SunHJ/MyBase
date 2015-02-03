#include "SocketAcceptor.h"

#ifdef PLATFORM_OS_WINDOWS
VOID WINAPI IOCompletionCallBack(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
	PAsyncSocketStream pAsyncSocketStream = CONTAINING_RECORD(lpOverlapped, AsyncSocketStream, m_wsaOverlapped);
	ASSERT(pAsyncSocketStream);

	if (dwNumberOfBytesTransfered == 0)
		dwErrorCode = WSAEDISCON;

	pAsyncSocketStream->OnRecvCompleted(dwErrorCode, dwNumberOfBytesTransfered, lpOverlapped);
}
#endif

NonBlockSocketAcceptor::NonBlockSocketAcceptor() : m_hListenSocket(INVALID_SOCKET), m_strIpAddress(""), m_usPort(0)
{
	//
}

NonBlockSocketAcceptor::~NonBlockSocketAcceptor()
{
	//
}

BOOL NonBlockSocketAcceptor::Init(CONST STRING &strIpAddress, CONST USHORT &usPort)
{
	m_usPort = usPort;
	m_strIpAddress = strIpAddress;

	BOOL bRetCode = FALSE;

	m_hListenSocket = g_CreateListenSocket(m_strIpAddress.c_str(), m_usPort);
	PROCESS_ERROR(INVALID_SOCKET != m_hListenSocket);

	bRetCode = g_SetSocketNonBlock(m_hListenSocket);
	PROCESS_ERROR(bRetCode);

#ifdef PLATFORM_OS_LINUX
	{ // linux
		typedef void(*SignalHandlerPointer)(int);
		SignalHandlerPointer pSignalHandler = SIG_ERR;

		m_nEpollHandle = -1;
		m_SocketEventVector.clear();

		pSignalHandler = signal(SIGPIPE, SIG_IGN);
		PROCESS_ERROR(pSignalHandler != SIG_ERR);  // write when connection reset.

		m_nEpollHandle = epoll_create(256);
		PROCESS_ERROR(m_nEpollHandle != -1);
	}
#endif
	return TRUE;

Exit0:
	g_CloseSocket(m_hListenSocket);
	return FALSE;
}

BOOL NonBlockSocketAcceptor::AttachSocketStreamQueue(SPAsyncSocketStreamQueue &spQueue)
{
	m_spSocketStreamQueue = spQueue;
	return TRUE;
}

BOOL NonBlockSocketAcceptor::UnInit()
{
	BOOL bRetCode = FALSE;

	bRetCode = g_CloseSocket(m_hListenSocket);
	CHECK_RETURN_BOOL(bRetCode);

	return TRUE;
}

BOOL NonBlockSocketAcceptor::Wait(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray)
{
	BOOL bRetCode = FALSE;

	bRetCode = WaitProcessAccept(MAX_WAIT_ACCEPT_EVENT, nEventCount, spEventArray);
	PROCESS_ERROR_QUIET(bRetCode);

	bRetCode = WaitClientRequet(MAX_SOCKET_EVENT, nEventCount, spEventArray);
	PROCESS_ERROR_QUIET(bRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL NonBlockSocketAcceptor::WaitProcessAccept(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray)
{
	BOOL bResult, bRetCode;
	INT nSuccessEventCount, nErrorCode;

	CHECK_RETURN_BOOL(nMaxEventCount > 0 && spEventArray);
	CHECK_RETURN_BOOL(nEventCount <= nMaxEventCount);

	bResult = TRUE;
	nSuccessEventCount = 0;
	while (nEventCount < nMaxEventCount)
	{
		nErrorCode = 0;
		PAsyncSocketStream pAsyncSocketStream = NULL;
		bRetCode = _AcceptToAsyncSocketStream(pAsyncSocketStream, &nErrorCode);
		if (FALSE == bRetCode)
		{
			// 没有连接请求
			if (0 == nErrorCode)
				break;

			// 出错，跳转到Exit0
			bResult = FALSE;
			PROCESS_ERROR_QUIET(FALSE);
		}
		// 有新的客户端连入
		SPAsyncSocketStream spAsyncSocketStream(pAsyncSocketStream);
		if (m_spSocketStreamQueue)
			m_spSocketStreamQueue->AddClient(spAsyncSocketStream);

		spEventArray[nEventCount].m_nEventType = SOCKET_EVENT_ACCEPT;
		spEventArray[nEventCount].m_spAsyncSocketStream = spAsyncSocketStream;

		nEventCount++;
		nSuccessEventCount++;
	}

Exit0:
	if (!bResult)
	{
		while (nSuccessEventCount--)
		{
			nEventCount--;
			spEventArray[nEventCount].Reset();
		}
	}
	return bResult;
}


BOOL NonBlockSocketAcceptor::WaitClientRequet(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray)
{
	BOOL bReCode;
#ifdef PLATFORM_OS_LINUX
	CHECK_RETURN_BOOL(-1 != m_nEpollHandle);
#endif // PLATFORM_OS_WINDOWS
	bReCode = m_spSocketStreamQueue->Wait(nMaxEventCount, nEventCount, spEventArray, m_nEpollHandle);
	return bReCode;
}

// return TRUE:success(pErrorCode:1), FALSE: (*pErrorCode 0:No Request, -1:Error)
BOOL NonBlockSocketAcceptor::_AcceptToAsyncSocketStream(PAsyncSocketStream &pAsyncSocketStream, INT *pErrorCode /* = NULL */)
{
	INT nRetCode, nAddrLen;
	BOOL bResult = FALSE, bLoopFlag = TRUE, bRetCode;
	SOCKET hRemoteSocket = INVALID_SOCKET;
	struct sockaddr_in saRemoteAddr;
	CHECK_RETURN_BOOL(INVALID_SOCKET != m_hListenSocket);

	nAddrLen = sizeof(struct sockaddr_in);
	g_SetErrorCode(pErrorCode, 0);
	while (bLoopFlag)
	{
		g_ZeroMemory(&saRemoteAddr, nAddrLen);
		hRemoteSocket = ::accept(m_hListenSocket, (struct sockaddr *)&saRemoteAddr, &nAddrLen);

		if (SOCKET_ERROR == hRemoteSocket)
		{
			nRetCode = g_IsSocketCanRestore();
			if (nRetCode)
			{// interrupted system call, try again
				continue;
			}

			bResult = FALSE;
			nRetCode = g_IsSocketWouldBlock();
			if (nRetCode)
			{
				g_SetErrorCode(pErrorCode, 0);
				goto Exit0;
			}
			// return ErrorCode to Upper
			nRetCode = g_GetSocketLastError();
			g_SetErrorCode(pErrorCode, nRetCode);
			goto Exit0;
		}

		pAsyncSocketStream = ::new AsyncSocketStream();
		PROCESS_ERROR(NULL != pAsyncSocketStream);

		bRetCode = pAsyncSocketStream->Init(hRemoteSocket, ::inet_ntoa(saRemoteAddr.sin_addr), saRemoteAddr.sin_port);
		PROCESS_ERROR(bRetCode);

#ifdef PLATFORM_OS_WINDOWS															
		bRetCode = ::BindIoCompletionCallback((HANDLE)hRemoteSocket, IOCompletionCallBack, 0);
		PROCESS_ERROR(bRetCode);
#else	  
		{
			epoll_event ev;
			ev.events = EPOLLIN | EPOLLET;
			ev.data.ptr = (void *)pAsyncSocketStream;

			nRetCode = epoll_ctl(m_nEpollHandle, EPOLL_CTL_ADD, nSocket, &ev);
			PROCESS_ERROR(nRetCode >= 0);
		}
#endif // PLATFORM_OS_WINDOWS

		bLoopFlag = FALSE;
	}

	bResult = TRUE;

Exit0:
	if (!bResult)
	{
		if (NULL != pAsyncSocketStream)
		{
			pAsyncSocketStream->UnInit();
			g_SafelyDeletePtr(pAsyncSocketStream);
		}
	}
	return bResult;
}

//#ifdef PLATFORM_OS_WINDOWS
// BOOL NonBlockSocketAcceptor::_WaitProcessOrDestory(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray)
// {
// }
// 
// 
// #else	   
// BOOL NonBlockSocketAcceptor::_WaitProcessRecv(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray)
// {
// 
// }
//#endif //PLATFORM_OS_WINDOWS