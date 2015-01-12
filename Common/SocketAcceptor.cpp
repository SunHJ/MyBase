#include "SocketAcceptor.h"

#ifdef PLATFORM_OS_WINDOWS
	typedef int SockLen;
#else
	typedef socklen_t SockLen;
#endif // PLATFORM_OS_WINDOWS

// 1  : indicates sucess
// 0  : non waiting connection or operations will complete later
// -1 : accept failed. this indicates a serious system error
inline INT g_AcceptToAsyncSocketStream(SOCKET hListenSocket, PAsyncSocketStream &pAsyncSocketStream, INT *pErrorCode /* = NULL */)
{
	CHECK_RETURN_BOOL(INVALID_SOCKET != hListenSocket);
	INT nResult = 1;
	g_SetErrorCode(pErrorCode, 0);
	struct sockaddr_in saRemoteAddr;
	SOCKET hRemoteSocket = INVALID_SOCKET;
	SockLen nAddrLen = sizeof(struct sockaddr_in);
	BOOL bLoopFlag = TRUE, bRetCode = FALSE;

	while (bLoopFlag)
	{
		g_ZeroMemory(&saRemoteAddr, nAddrLen);
		hRemoteSocket = (SOCKET)::accept(hListenSocket, (struct sockaddr *)&saRemoteAddr, &nAddrLen);

		if (SOCKET_ERROR == hRemoteSocket)
		{
			if (g_IsSocketCanRestore())
			{// interrupted system call, try again
				continue;
			}

			if (g_IsSocketWouldBlock())
			{
				nResult = 0;
				g_SetErrorCode(pErrorCode, 0);
				goto Exit0;
			}
			nResult = -1;
			// save other error code
			INT nErrorCode = g_GetSocketLastError();
			g_SetErrorCode(pErrorCode, nErrorCode);
			goto Exit0;
		}

        g_SetSocketNonBlock(hRemoteSocket, pErrorCode);

		pAsyncSocketStream = ::new AsyncSocketStream();
		PROCESS_ERROR(NULL != pAsyncSocketStream);

		bRetCode = pAsyncSocketStream->Init(hRemoteSocket, ::inet_ntoa(saRemoteAddr.sin_addr), saRemoteAddr.sin_port);
		PROCESS_ERROR(bRetCode);

		break;
	}

	return nResult;
Exit0:
	if (NULL != pAsyncSocketStream)
	{
		pAsyncSocketStream->UnInit();
		g_SafelyDeletePtr(pAsyncSocketStream);
	}
	g_SetErrorCode(pErrorCode, -1);
	return nResult;
}

inline INT g_AcceptToAsyncSocketStream(SOCKET hListenSocket, SPAsyncSocketStream spAsyncSocketStream, INT *pErrorCode /* = NULL */)
{
	BOOL bRetCode = FALSE;
	PAsyncSocketStream pAsyncSocketStream = NULL;
	bRetCode = g_AcceptToAsyncSocketStream(hListenSocket, pAsyncSocketStream, pErrorCode);
	spAsyncSocketStream = SPAsyncSocketStream(pAsyncSocketStream);
	return bRetCode;
}

NonBlockSocketAcceptor::NonBlockSocketAcceptor(): m_hListenSocket(INVALID_SOCKET),
	m_strIpAddress(""), m_usPort(0)
{
	//
}

NonBlockSocketAcceptor::~NonBlockSocketAcceptor()
{
	//
}


/*Client Socket Acceptor 初始化 成功:返回TRUE 失败:返回FALSE*/
BOOL NonBlockSocketAcceptor::Init( CONST STRING &strIpAddress, CONST USHORT &usPort )
{
	m_usPort = usPort;
	m_strIpAddress = strIpAddress;

	BOOL bRetCode = FALSE;

	m_hListenSocket = g_CreateListenSocket(m_strIpAddress.c_str(), m_usPort);
	PROCESS_ERROR(INVALID_SOCKET != m_hListenSocket);

	bRetCode = g_SetSocketNonBlock(m_hListenSocket);
	PROCESS_ERROR(bRetCode);

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

BOOL NonBlockSocketAcceptor::WaitAccept( INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray )
{
	BOOL bRetCode = FALSE;

	bRetCode = WaitProcessAccept(nMaxEventCount, nEventCount, spEventArray);
	PROCESS_ERROR(bRetCode);

	return TRUE;
Exit0:
	return FALSE;
}

BOOL NonBlockSocketAcceptor::WaitProcessAccept( INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray )
{
	BOOL bResult = FALSE;
	INT nRetCode = FALSE;
	INT nSuccessEventCount = 0;

	CHECK_RETURN_BOOL(nMaxEventCount>0 && spEventArray);
	CHECK_RETURN_BOOL(nEventCount <= nMaxEventCount);

	while (nEventCount < nMaxEventCount)
	{
		INT nErrorCode = 0;
		PAsyncSocketStream pAsyncSocketStream = NULL;
		nRetCode = g_AcceptToAsyncSocketStream(m_hListenSocket, pAsyncSocketStream, &nErrorCode);

		if (1 == nRetCode)
		{	
			SPAsyncSocketStream spAsyncSocketStream(pAsyncSocketStream);
			if (m_spSocketStreamQueue)
				m_spSocketStreamQueue->PushBack(spAsyncSocketStream);

			spEventArray[nEventCount].m_nEventType = ASYNC_SOCKET_EVENT_ACCEPT;
			spEventArray[nEventCount].m_spAsyncSocketStream = spAsyncSocketStream;

			nEventCount++;
			nSuccessEventCount++;
		}
		else if (0 == nRetCode)
		{
			bResult = TRUE;
			break;
		}
		else
		{
			goto Exit0;
		}
	}

	return bResult;
Exit0:
	while (nSuccessEventCount--)
	{
		nEventCount--;
		spEventArray[nEventCount].Reset();
	}
	return FALSE;
}
