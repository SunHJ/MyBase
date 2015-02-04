#include "SocketWrapper.h"
#include "SuperServer.h"


SuperServer::SuperServer()
{
	SINGLETON_GET_PTR(NetService)->Strat();
	
	m_bLoopFlag = FALSE;
	m_threadID = 0;
	m_spDataBuffer = SPDynamicBuffer(new DynamicBuffer());

#ifdef PLATFORM_OS_WINDOWS
	m_hThread = INVALID_HANDLE_VALUE;
#endif // PLATFORM_OS_WINDOWS
}


SuperServer::~SuperServer()
{
	SINGLETON_GET_PTR(NetService)->Stop();

	Stop();
}

BOOL SuperServer::Init(CONST STRING &strIp, CONST USHORT &usPort)
{					 
	BOOL bRetCode = FALSE; 

	PNonBlockSocketAcceptor pSocketAcceptor = ::new NonBlockSocketAcceptor();
	PROCESS_ERROR(NULL != pSocketAcceptor);
	m_spAcceptor = SPNonBlockSocketAcceptor(pSocketAcceptor);

	PAsyncSocketStreamQueue pSocketQueue = ::new AsyncSocketStreamQueue();
	PROCESS_ERROR(NULL != pSocketQueue);
	m_spStreamQueue = SPAsyncSocketStreamQueue(pSocketQueue);

	PAsyncSocketEvent pSocketEvent = new AsyncSocketEvent[MAX_SOCKET_EVENT];
	PROCESS_ERROR(NULL != pSocketEvent);
	m_spEventArray = SPAsyncSocketEventArray(pSocketEvent);

	bRetCode = m_spAcceptor->Init(strIp, usPort);
	PROCESS_ERROR(bRetCode);

	bRetCode = m_spAcceptor->AttachSocketStreamQueue(m_spStreamQueue);
	PROCESS_ERROR(bRetCode);

	return TRUE;
Exit0:
	g_SafelyDeletePtr(pSocketAcceptor);
	g_SafelyDeletePtr(pSocketQueue);
	g_SafelyDeleteArrayPtr(pSocketEvent);
	return FALSE;
}

VOID SuperServer::UnInit()
{
	m_spAcceptor->UnInit();
	m_spStreamQueue->CloseAll();
}

#ifdef PLATFORM_OS_WINDOWS
THREAD_FUNC_RET_TYPE __stdcall SuperServer::ThreadFunction(VOID *pValue)
#else
THREAD_FUNC_RET_TYPE KThread::ThreadFunction(VOID *pValue)
#endif // PLATFORM_OS_WINDOWS
{
	SuperServer* pThisAgency = (SuperServer*)pValue;
	return (THREAD_FUNC_RET_TYPE)(pThisAgency ? pThisAgency->Activate() : 0);
}

BOOL SuperServer::Start()
{
	m_bLoopFlag = TRUE;
	BOOL nResult = 0;
#ifdef PLATFORM_OS_WINDOWS
	if (m_hThread <= 0 || INVALID_HANDLE_VALUE == m_hThread)
	{
		m_hThread = (HANDLE)::_beginthreadex(0, 0, ThreadFunction, (VOID *)this, 0, &m_threadID);
	}
	nResult = m_threadID > 0;
#else
	int iRetCode = 0;
	if (m_threadID <= 0) {
		iRetCode = ::pthread_create(&m_threadID, NULL, ThreadFunction, (VOID *)this);
	}

	nResult = nRetCode == 0;
#endif	//PLATFORM_OS_WINDOWS

	printf("SuperServer (Port:%d) Start Success \n", m_spAcceptor->GetServerPort());
	return nResult;
}

VOID SuperServer::Stop()
{
	m_bLoopFlag = FALSE;
#ifdef PLATFORM_OS_WINDOWS
	CHECK_RETURN_VOID_QUIET(m_threadID > 0 && m_hThread != INVALID_HANDLE_VALUE);
	BOOL bRetCode = ::TerminateThread(m_hThread, 0);
	if (bRetCode)
	{
		g_CloseHandle(m_hThread);
	}
#else
	::pthread_cancel(m_threadID);
#endif //PLATFORM_OS_WINDOWS
}

size_t SuperServer::GetClientCount()
{
	return m_spStreamQueue->GetCurStreamVectorLen();
}

BOOL SuperServer::Activate()
{
	BOOL bRetCode = FALSE;
	while (m_bLoopFlag)
	{
		bRetCode = ProcessNetEvent();
		CHECK_RETURN_BOOL(bRetCode);
	}
	return TRUE;  
}

BOOL SuperServer::ProcessNetEvent()
{
	INT nErrorCode = 0, nEventCount;
	BOOL bLoopFlag = TRUE;
	BOOL bRetCode = FALSE;

	while (bLoopFlag)
	{
		nEventCount = 0;   
		m_spAcceptor->Wait(nEventCount, m_spEventArray);

		if (0 == nEventCount)   // no event
			break;

		for (INT i = 0; i < nEventCount; i++)
		{
			PAsyncSocketStream &pAsyncSocketStream = m_spEventArray[i].m_pAsyncSocketStream;
			switch (m_spEventArray[i].m_nEventType)
			{
			case SOCKET_EVENT_ACCEPT:
				{
					ProcessNewConnect(pAsyncSocketStream);
#ifdef PLATFORM_OS_WINDOWS
					bRetCode = pAsyncSocketStream->TryToActiveNextRecv();
					if (-1 == bRetCode)
					{
						ProcessClentClose(pAsyncSocketStream);
					}
#endif // PLATFORM_OS_WINDOWS
				}
				break;
			case SOCKET_EVENT_IN:
				{
					ProcessAllCompletePackage(pAsyncSocketStream);
				}
				break;
			case SOCKET_EVENT_CLOSE:
				{
					ProcessClentClose(pAsyncSocketStream);
				}
				break;
			default:
				break;
			}

			m_spEventArray[i].Reset();
		}
	}
	return TRUE;
}

BOOL SuperServer::ProcessAllCompletePackage(PAsyncSocketStream &pAsyncSocketStream)
{
	INT nErrorCode = 0;
	BOOL bResult = TRUE;
	BOOL bRetCode = FALSE;
	BOOL bLoopFlag = TRUE;

	while (bLoopFlag)
	{
		m_spDataBuffer->Reset();
		bRetCode = pAsyncSocketStream->Recv(m_spDataBuffer, &nErrorCode);
		if (1 == bRetCode)
		{
			ProcessNetMessage(pAsyncSocketStream, m_spDataBuffer);
		}

		if (-1 == bRetCode)
		{// a serious error was occured
			ProcessClentClose(pAsyncSocketStream);
			m_spStreamQueue->DelClient(pAsyncSocketStream);
			bResult = FALSE;
			bLoopFlag = FALSE;
		}

		if (0 == bRetCode)
			break;
	}
	return TRUE;  
}

VOID SuperServer::ProcessNewConnect(PAsyncSocketStream &pSocketStream)
{
	printf("new client(%s:%d) in...\n", pSocketStream->GetRemoteIp().c_str(), pSocketStream->GetRemotePort());
	/*		*********		*********		*********		*********		*********		*********		*********/
}

VOID SuperServer::ProcessNetMessage(PAsyncSocketStream &pSocketStream, SPDynamicBuffer &spBuffer)
{
	printf("client(%s %d) Msg in DataSize:%d\n", pSocketStream->GetRemoteIp().c_str(), pSocketStream->GetRemotePort(), spBuffer->GetUsedSize());
	/*		*********		*********		*********		*********		*********		*********		*********/
}

VOID SuperServer::ProcessClentClose(PAsyncSocketStream &pSocketStream)
{
	printf("client(%s %d) close\n", pSocketStream->GetRemoteIp().c_str(), pSocketStream->GetRemotePort());
	/*		*********		*********		*********		*********		*********		*********		*********/
}