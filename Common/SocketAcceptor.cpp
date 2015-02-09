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
#else
BOOL RegisterEpollCtrl(INT nEpollHandle, PAsyncSocketStream &pSocketSteam)
{
	BOOL bResult = FALSE;
	INT nRetCode = 0;
    SOCKET sConnFd;
	epoll_event ev;
	PROCESS_ERROR(-1 != nEpollHandle);
	PROCESS_ERROR(pSocketSteam);

    sConnFd = pSocketSteam->GetSocket();
    PROCESS_ERROR(INVALID_SOCKET != sConnFd);
    
    ev.data.fd = sConnFd;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.ptr = (void *)pSocketSteam;
	nRetCode = epoll_ctl(nEpollHandle, EPOLL_CTL_ADD, sConnFd, &ev);
	PROCESS_ERROR(nRetCode == 0);

	bResult = TRUE;

Exit0:
	return bResult;
}

BOOL UnRegisterEpollCtrl(INT nEpollHandle, PAsyncSocketStream &pSocketSteam)
{
	BOOL bResult = FALSE;
	INT nRetCode = 0;
    SOCKET sConnFd;
	epoll_event ev;
	PROCESS_ERROR(-1 != nEpollHandle);
	PROCESS_ERROR(pSocketSteam);

    sConnFd = pSocketSteam->GetSocket();
    PROCESS_ERROR(INVALID_SOCKET != sConnFd);
    
    ev.data.fd = sConnFd;
	ev.events = EPOLLIN | EPOLLET;
	//ev.data.ptr = (void *)pSocketSteam;
	nRetCode = epoll_ctl(nEpollHandle, EPOLL_CTL_DEL, sConnFd, &ev);
	PROCESS_ERROR(nRetCode == 0);

	bResult = TRUE;

Exit0:
	return bResult;
}

VOID EpollRecvThreadFun(PVOID pParam)
{
	ASSERT(pParam);
	NonBlockSocketAcceptor* pAcceptor = (NonBlockSocketAcceptor*)pParam;
	pAcceptor->MainLoopRecv();
}

#endif

NonBlockSocketAcceptor::NonBlockSocketAcceptor() : m_hListenSocket(INVALID_SOCKET), m_strIpAddress(""), m_usPort(0)
{
#ifdef PLATFORM_OS_LINUX
	m_bLoopFlag = FALSE; 
	m_nEpollHandle = -1;
	m_nHeadPos = 0;
	m_nTailPos = 0;	
	for (int i = 0; i < MAX_SOCKET_EVENT; i++)
	{
		WaitQueue[i] = NULL;
	}
#endif // PLATFORM_OS_LINUX		
}

NonBlockSocketAcceptor::~NonBlockSocketAcceptor()
{
	UnInit();
}

BOOL NonBlockSocketAcceptor::Init(CONST STRING &strIpAddress, CONST USHORT &usPort)
{
	BOOL bResult = FALSE;
	BOOL bRetCode = FALSE;

	m_usPort = usPort;
	m_strIpAddress = strIpAddress;

	m_hListenSocket = g_CreateListenSocket(m_strIpAddress.c_str(), m_usPort);
	PROCESS_ERROR(INVALID_SOCKET != m_hListenSocket);

	bRetCode = g_SetSocketNonBlock(m_hListenSocket);
	PROCESS_ERROR(bRetCode);

#ifdef PLATFORM_OS_LINUX
	m_nEpollHandle = epoll_create(MAX_SOCKET_EVENT);
	PROCESS_ERROR(m_nEpollHandle != -1);

	m_bLoopFlag = TRUE;
	bRetCode = m_cRecvThread.Start(&EpollRecvThreadFun, (VOID*)this);
	PROCESS_ERROR(bRetCode);

#endif
	bResult = TRUE;

Exit0:
	if (!bResult)
	{
		g_CloseSocket(m_hListenSocket);
	}
	return bResult;
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
#ifdef PLATFORM_OS_LINUX  
	m_bLoopFlag = FALSE;
    m_Semap.ReleaseSemaphore();
	m_cRecvThread.Stop();

	m_nHeadPos = 0;
	m_nTailPos = 0;
	for (int i = 0; i < MAX_SOCKET_EVENT; i++)
	{
		WaitQueue[i] = NULL;
	}

	if (-1 != m_nEpollHandle)
	{
		INT nRetCode = ::close(m_nEpollHandle);
		CHECK_RETURN_BOOL(0 == nRetCode);
		m_nEpollHandle = -1;
	}
#endif // PLATFORM_OS_LINUX

	return TRUE;
}

BOOL NonBlockSocketAcceptor::Wait(INT &nEventCount, SPAsyncSocketEventArray spEventArray)
{
	BOOL bRetCode = FALSE;

	bRetCode = WaitProcessAccept(MAX_WAIT_ACCEPT_EVENT, nEventCount, spEventArray);
	PROCESS_ERROR_QUIET(bRetCode);
#ifdef PLATFORM_OS_LINUX
    _EpollWaitProcess();
#endif
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

	CHECK_RETURN_BOOL(nEventCount >= 0 && nEventCount < nMaxEventCount);
	CHECK_RETURN_BOOL(spEventArray);

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
		if (m_spSocketStreamQueue)
			m_spSocketStreamQueue->AddClient(pAsyncSocketStream);

		spEventArray[nEventCount].m_nEventType = SOCKET_EVENT_ACCEPT;
		spEventArray[nEventCount].m_pAsyncSocketStream = pAsyncSocketStream;

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
	BOOL bReCode = FALSE;  
	bReCode = m_spSocketStreamQueue->Wait(nMaxEventCount, nEventCount, spEventArray);
	return bReCode;
}

// return TRUE:success(pErrorCode:1), FALSE: (*pErrorCode 0:No Request, -1:Error)
BOOL NonBlockSocketAcceptor::_AcceptToAsyncSocketStream(PAsyncSocketStream &pAsyncSocketStream, INT *pErrorCode /* = NULL */)
{
	INT nRetCode;
	BOOL bResult = FALSE, bLoopFlag = TRUE, bRetCode;
	SOCKET hRemoteSocket = INVALID_SOCKET;
	SockLen nAddrLen = sizeof(struct sockaddr_in);
	struct sockaddr_in saRemoteAddr;
	CHECK_RETURN_BOOL(INVALID_SOCKET != m_hListenSocket);

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
			{	// no connect request
				g_SetErrorCode(pErrorCode, 0);
				PROCESS_ERROR_QUIET(FALSE);
			}

			// return ErrorCode to Upper
			g_SetErrorCode(pErrorCode, -1);
			PROCESS_ERROR_QUIET(FALSE);
		}

		pAsyncSocketStream = ::new AsyncSocketStream();
		PROCESS_ERROR(NULL != pAsyncSocketStream);

		bRetCode = pAsyncSocketStream->Init(hRemoteSocket, ::inet_ntoa(saRemoteAddr.sin_addr), saRemoteAddr.sin_port);
		PROCESS_ERROR(bRetCode);

#ifdef PLATFORM_OS_WINDOWS															
		bRetCode = ::BindIoCompletionCallback((HANDLE)hRemoteSocket, IOCompletionCallBack, 0);
#else	
		bRetCode = RegisterEpollCtrl(m_nEpollHandle, pAsyncSocketStream);
#endif // PLATFORM_OS_WINDOWS
		PROCESS_ERROR(bRetCode);

		// 成功接收一个客户端，结束循环
		bLoopFlag = FALSE;
	}

	bResult = TRUE;

Exit0:
	if (!bResult)
	{
		if (NULL != pAsyncSocketStream)
		{
			g_SafelyDeletePtr(pAsyncSocketStream);
		}
	}
	return bResult;
}

#ifdef PLATFORM_OS_LINUX
BOOL NonBlockSocketAcceptor::_EpollWaitProcess()
{
	BOOL bResult = FALSE;
	INT nRetCode = 0, nRemainEventCount;
	PAsyncSocketStream pSocketStream = NULL;

	PROCESS_ERROR(-1 != m_nEpollHandle);
	nRemainEventCount = m_spSocketStreamQueue->GetCurStreamVectorLen();

	nRetCode = epoll_wait(m_nEpollHandle, m_EpollEvents, nRemainEventCount, 0);
	
    for (int i = 0; i < nRetCode; i++)
	{
		pSocketStream = (PAsyncSocketStream)(m_EpollEvents[i].data.ptr);
		if (m_EpollEvents[i].events&EPOLLIN)
		{   
            m_nHeadPos = (m_nHeadPos + 1) % MAX_SOCKET_EVENT;
            WaitQueue[m_nHeadPos] = pSocketStream;
            m_Semap.ReleaseSemaphore();

            //nRetCode = pSocketStream->TryEpollRecv();
            //if (nRetCode <= 0)
            //{
            //    UnRegisterEpollCtrl(m_nEpollHandle, pSocketStream);
            //}

		}
	}
    bResult = TRUE;

Exit0:
	return bResult;
}

VOID NonBlockSocketAcceptor::MainLoopRecv()
{
	INT nRetCode = 0;
	PAsyncSocketStream pCurSocket = NULL;
	while (m_bLoopFlag)
	{
        nRetCode = m_Semap.WaitSemaphore();

        m_nTailPos = (m_nTailPos + 1) % MAX_SOCKET_EVENT;
		pCurSocket = WaitQueue[m_nTailPos];
		if (NULL == pCurSocket)
			continue;

		nRetCode = pCurSocket->TryEpollRecv();
        if (nRetCode <= 0)
        {
            UnRegisterEpollCtrl(m_nEpollHandle, pCurSocket);
        }

		WaitQueue[m_nTailPos] = NULL;
	}
}

#endif // PLATFORM_OS_LINUX
