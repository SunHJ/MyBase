#include "SelecterServer.h"


// 1 : indicates sucess
// 0 : non waiting connection or operations will complete later
//-1 : accept failed. this indicates a serious system error
inline INT g_AcceptSocketStream(SOCKET hListenSocket, PSocketStream &pAsyncSocketStream, INT *pErrorCode /* = NULL */)
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

		pAsyncSocketStream = ::new SocketStream();
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

SelecterServer::SelecterServer() : m_bStop(TRUE)
{
#ifdef PLATFORM_OS_WINDOWS
	ASSERT(SINGLETON_GET_PTR(NetService)->Start());
#endif // PLATFORM_OS_WINDOWS
	PDynamicBuffer pBuffer = ::new DynamicBuffer(DEFAULT_BUFFER_SIZE);
	m_spBuffer = SPDynamicBuffer(pBuffer);

	PSocketStreamQueue pSocketStreamQueue = new SocketStreamQueue();
	m_spConnectSocket = SPSocketStreamQueue(pSocketStreamQueue);
	
	PSocketEvent pEventArray = ::new SocketEvent[MAX_SOCKET_EVENT];
	m_spEventArray = SPSocketEventArray(pEventArray);
}

SelecterServer::~SelecterServer()
{
#ifdef PLATFORM_OS_WINDOWS
	SINGLETON_GET_PTR(NetService)->Stop();
#endif // PLATFORM_OS_WINDOWS
}

BOOL SelecterServer::Init(CONST STRING strIpAddress, CONST USHORT usPort)
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

VOID SelecterServer::UnInit()
{
	if (!m_bStop)
	{
		Stop();
	}
	m_usPort = 0;
	m_strIpAddress = "";
	g_CloseSocket(m_hListenSocket);
}

BOOL SelecterServer::Start()
{
	BOOL bReCode = FALSE;

	// Start Work Thread
	m_bStop = FALSE;
	bReCode = m_cThread.Start(&SelecterServer::WorkThreadFun, (VOID*)this);
	CHECK_RETURN_BOOL(bReCode);
	printf("Server(Port:%d) Start Success \n", m_usPort);
	return TRUE;
}

VOID SelecterServer::Stop()
{
	m_bStop = TRUE;
	if (!m_SemStop.WaitSemaphore(10 * 1000))
	{
		m_cThread.Stop();
	}
	m_spConnectSocket->CloseAll();
}

INT SelecterServer::GetClientCount()
{
	size_t uCount = m_spConnectSocket->GetCurStreamVectorLen();
	return static_cast<INT>(uCount);
}

VOID SelecterServer::ProcessNewConnect(SPSocketStream &spSocketStream)
{
	printf("new client(%s:%d) in...\n", spSocketStream->GetRemoteIp().c_str(), spSocketStream->GetRemotePort());
/*		*********		*********		*********		*********		*********		*********		*********/
}

VOID SelecterServer::ProcessNetMessage(SPSocketStream &spSocketStream)
{
	printf("client(%s %d) Msg in ", spSocketStream->GetRemoteIp().c_str(), spSocketStream->GetRemotePort());
/*		*********		*********		*********		*********		*********		*********		*********/
	INT nErrorCode = 0;
	BOOL nReCode = FALSE;
	m_spBuffer->Reset();
	nReCode = spSocketStream->Recv(m_spBuffer, &nErrorCode);
	if (1 == nReCode)	 // recv success
	{
		printf("datalen:%d\n", m_spBuffer->GetUsedSize());
	}
	else if (0 == nReCode)	// client close when recv
	{
		printf("close\n");
		m_spConnectSocket->Remove(spSocketStream);
		ProcessClentClose(spSocketStream);
	}
	else
	{
		printf("error!!! num=%d\n", nErrorCode);
	}
}

VOID SelecterServer::ProcessClentClose(SPSocketStream &spSocketStream)
{
	printf("client(%s %d) close\n", spSocketStream->GetRemoteIp().c_str(), spSocketStream->GetRemotePort());
/*		*********		*********		*********		*********		*********		*********		*********/
}

VOID SelecterServer::MainLoop()
{
	INT nEventCount = 0;
	while (!m_bStop)
	{
		nEventCount = 0;
		_WaitClientAccept(MAX_WAIT_ACCEPT_EVENT, nEventCount, m_spEventArray);
		m_spConnectSocket->SelectWait(MAX_SOCKET_EVENT, nEventCount, m_spEventArray);
		// 没有客户端请求
		if (0 == nEventCount)
		{
			g_MilliSleep(50);
			continue;
		}
		// 依次处理客户端链接、网络包请求
		for (INT i = 0; i < nEventCount; i++)
		{
			SPSocketStream &spSocketStream = m_spEventArray[i].m_spAsyncSocketStream;
			switch (m_spEventArray[i].m_nEventType)
			{
			case SOCKET_EVENT_ACCEPT:
				{
					ProcessNewConnect(spSocketStream);
				}
				break;
			case SOCKET_EVENT_IN:		  
				{
					ProcessNetMessage(spSocketStream);
				}
				break;
			case SOCKET_EVENT_CLOSE:
				{
					ProcessClentClose(spSocketStream);
				}
				break;
			default:
				break;
			}
			m_spEventArray[i].Reset();
		}
	}
	// 工作线程正常结束，发出线程停止信号
	m_SemStop.ReleaseSemaphore();
}

VOID SelecterServer::WorkThreadFun(VOID* pParam)
{
	ASSERT(pParam);
	SelecterServer* pServer = (SelecterServer*)pParam;
	pServer->MainLoop();
}

BOOL SelecterServer::_WaitClientAccept(INT nMaxEventCount, INT &nEventCount, SPSocketEventArray spEventArray)
{
	BOOL bResult = FALSE;
	INT nRetCode = FALSE;
	INT nSuccessEventCount = 0;

	CHECK_RETURN_BOOL(nMaxEventCount > 0 && spEventArray);
	CHECK_RETURN_BOOL(nEventCount <= nMaxEventCount);

	while (nEventCount < nMaxEventCount)
	{
		INT nErrorCode = 0;
		PSocketStream pAsyncSocketStream = NULL;
		nRetCode = g_AcceptSocketStream(m_hListenSocket, pAsyncSocketStream, &nErrorCode);

		if (1 == nRetCode)
		{
			SPSocketStream spAsyncSocketStream(pAsyncSocketStream);
			if (m_spConnectSocket)
				m_spConnectSocket->PushBack(spAsyncSocketStream);

			spEventArray[nEventCount].m_nEventType = SOCKET_EVENT_ACCEPT;
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
	return bResult;
}


