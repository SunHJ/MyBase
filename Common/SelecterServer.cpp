#include "SelecterServer.h"

SelecterServer::SelecterServer() : m_bStop(TRUE)
{
#ifdef PLATFORM_OS_WINDOWS
	ASSERT(SINGLETON_GET_PTR(NetService)->Strat());
#endif // PLATFORM_OS_WINDOWS
	PDynamicBuffer pBuffer = ::new DynamicBuffer(DEFAULT_BUFFER_SIZE);
	m_spBuffer = SPDynamicBuffer(pBuffer);

	PAsyncSocketStreamQueue pSocketStreamQueue = new AsyncSocketStreamQueue();
	m_spConnectSocket = SPAsyncSocketStreamQueue(pSocketStreamQueue);
	
	PAsyncSocketEvent pEventArray = ::new AsyncSocketEvent[MAX_SOCKET_EVENT];
	m_spEventArray = SPAsyncSocketEventArray(pEventArray);

	PNonBlockSocketAcceptor pSocketAcceptor = new NonBlockSocketAcceptor();
	m_spSocketAcceptor = SPNonBlockSocketAcceptor(pSocketAcceptor);
	// SocketAccepter里面有 m_spConnectSocket 的引用
	m_spSocketAcceptor->AttachSocketStreamQueue(m_spConnectSocket);
}

SelecterServer::~SelecterServer()
{
#ifdef PLATFORM_OS_WINDOWS
	SINGLETON_GET_PTR(NetService)->Stop();
#endif // PLATFORM_OS_WINDOWS
}

BOOL SelecterServer::Start(STRING szIp, USHORT uPort)
{
	BOOL bReCode = FALSE;

	bReCode = m_spSocketAcceptor->Init(szIp, uPort);
	PROCESS_ERROR(bReCode);
	
	// Start Work Thread
	m_bStop = FALSE;
	bReCode = m_cThread.Start(&SelecterServer::WorkThreadFun, (VOID*)this);
	PROCESS_ERROR(bReCode);

	printf("Server(Port:%d) Start Success \n", uPort);

Exit0:
	if (m_bStop)
	{
		m_spSocketAcceptor->UnInit();
	}
	return TRUE;
}

VOID SelecterServer::Stop()
{
	m_bStop = TRUE;
	if (!m_SemStop.WaitSemaphore(10 * 1000))
	{
		m_cThread.Stop();
	}

	m_spSocketAcceptor->UnInit();
	m_spConnectSocket->CloseAll();
}

INT SelecterServer::GetClientCount()
{
	size_t uCount = m_spConnectSocket->GetCurStreamVectorLen();
	return static_cast<INT>(uCount);
}

VOID SelecterServer::ProcessNewConnect(SPAsyncSocketStream &spSocketStream)
{
	printf("new client(%s:%d) in...\n", spSocketStream->GetRemoteIp().c_str(), spSocketStream->GetRemotePort());
	/*		*********		*********		*********		*********		*********		*********		*********/
}

VOID SelecterServer::ProcessNetMessage(SPAsyncSocketStream &spSocketStream)
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

VOID SelecterServer::ProcessClentClose(SPAsyncSocketStream &spSocketStream)
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
		m_spSocketAcceptor->WaitAccept(MAX_WAIT_ACCEPT_EVENT, nEventCount, m_spEventArray);
		m_spConnectSocket->SelectWait(MAX_SOCKET_EVENT, nEventCount, m_spEventArray);

		if (0 == nEventCount)
		{
			continue;
		}

		for (INT i = 0; i < nEventCount; i++)
		{
			SPAsyncSocketStream &spSocketStream = m_spEventArray[i].m_spAsyncSocketStream;
			switch (m_spEventArray[i].m_nEventType)
			{
			case ASYNC_SOCKET_EVENT_ACCEPT:
				{
					ProcessNewConnect(spSocketStream);
				}
				break;
			case ASYNC_SOCKET_EVENT_IN:		  
				{
					ProcessNetMessage(spSocketStream);
				}
				break;
			case ASYNC_SOCKET_EVENT_CLOSE:
				{
					ProcessClentClose(spSocketStream);
				}
				break;
			default:
				break;
			}
			m_spEventArray[i].Reset();
		}
		g_MilliSleep(50);
	}
	m_SemStop.ReleaseSemaphore();
}

VOID SelecterServer::WorkThreadFun(VOID* pParam)
{
	ASSERT(pParam);
	SelecterServer* pServer = (SelecterServer*)pParam;
	pServer->MainLoop();
}

