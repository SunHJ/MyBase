#include "Macro.h"
#include "Global.h"
#include "Thread.h"

#ifdef PLATFORM_OS_WINDOWS
CONST DWORD INVALID_DW_VALUE = static_cast<DWORD>(-1);
HANDLE IThread::GetHandle() CONST
{
	return m_hThread;
}

THREAD_FUNC_RET_TYPE WINAPI IThread::ThreadFunction(VOID *pValue)
#else
THREAD_FUNC_RET_TYPE IThread::ThreadFunction(VOID *pValue)
#endif // PLATFORM_OS_WINDOWS
{
	UINT                 uReCode = 0;
	THREAD_FUNC_RET_TYPE reValue = 0;
	IThread* pThisThread = (IThread*)pValue;
	if (NULL != pThisThread)
	{
		while (true)
		{
			pThisThread->PreRun();
			uReCode = pThisThread->Run();
		}
	}
	return reValue;
}

IThread::IThread()
{
	m_eFlag = eNone;
	m_threadID = 0;

#ifdef PLATFORM_OS_WINDOWS
	m_hThread = NULL;
#else
	pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
#endif //PLATFORM_OS_WINDOWS
}

IThread::~IThread()
{
	IThread::Stop();
}

BOOL IThread::Init()
{
	return TRUE;
}

BOOL IThread::Start()
{
    BOOL bResult = FALSE;
	if (m_threadID <= 0)
	{
#ifdef PLATFORM_OS_WINDOWS
		m_hThread = (HANDLE)::_beginthreadex(0, 0, ThreadFunction, (VOID*)this, 0, &m_threadID);
#else
		::pthread_create(&m_threadID, NULL, ThreadFunction, (VOID*)this);
#endif // PLATFORM_OS_WINDOWS
        if (m_threadID > 0)
        {
            m_eFlag = eActive;
            bResult = TRUE;
        }
	}
	return bResult;
}

VOID IThread::PreRun()
{ 
#ifdef PLATFORM_OS_WINDOWS

#else 
    pthread_mutex_lock(&mutex);
    while(m_eFlag == eSuspend)
    {
		//会先解除之前的pthread_mutex_lock锁定的mtx，然后阻塞在等待对列里休眠，直到再次被唤醒
		//（大多数情况下是等待的条件成立而被唤醒，唤醒后，该进程会先锁定先pthread_mutex_lock(&mutex);，再读取资源
        pthread_cond_wait(&cond, &mutex); 
    }
    pthread_mutex_unlock(&mutex);
#endif // PLATFORM_OS_WINDOWS
}

BOOL IThread::Stop(DWORD dwExitCode /* = 0 */)
{
	BOOL bResult = FALSE;
	INT nRetCode = 0;
	CHECK_RETURN_BOOL_QUIET(m_threadID > 0);

#ifdef PLATFORM_OS_WINDOWS 
	CHECK_RETURN_BOOL_QUIET(m_hThread != INVALID_HANDLE_VALUE);
	nRetCode = ::TerminateThread(m_hThread, dwExitCode);
	if (nRetCode)
	{
		g_CloseHandle(m_hThread);
		m_eFlag = eNone;
		bResult = TRUE;
	}
#else
	nRetCode = ::pthread_cancel(m_threadID);
	if (nRetCode == 0)
	{	   
		m_eFlag = eNone;
		bResult = TRUE;
	}
#endif // PLATFORM_OS_WINDOWS
	return bResult;
}

BOOL IThread::SuspendThread()
{
	BOOL bResult = FALSE;
	CHECK_RETURN_BOOL_QUIET(m_threadID > 0);
	if (m_eFlag == eActive)
	{
#ifdef PLATFORM_OS_WINDOWS
		CHECK_RETURN_BOOL_QUIET(m_hThread != INVALID_HANDLE_VALUE);
		DWORD dwRetCode = ::SuspendThread(m_hThread);
		if (INVALID_DW_VALUE != dwRetCode)
		{
			m_eFlag = eSuspend;
			bResult = TRUE;
		}
#else  
		pthread_mutex_lock(&mutex);
		m_eFlag = eSuspend;
		bResult = TRUE;
		pthread_mutex_unlock(&mutex);

#endif // PLATFORM_OS_WINDOWS
	}

	return bResult;
}

BOOL IThread::ResumeThread()
{
	BOOL bResult = FALSE;
	CHECK_RETURN_BOOL_QUIET(m_threadID > 0);
	if (m_eFlag == eSuspend)
	{
#ifdef PLATFORM_OS_WINDOWS
		CHECK_RETURN_BOOL_QUIET(m_hThread != INVALID_HANDLE_VALUE);
		DWORD dwRetCode = ::ResumeThread(m_hThread);
		if (INVALID_DW_VALUE != dwRetCode)
		{
			m_eFlag = eActive;
			bResult = TRUE;
		}
#else
		pthread_mutex_lock(&mutex);
		pthread_cond_signal(&cond);
		m_eFlag = eActive;
		bResult = TRUE;
		pthread_mutex_unlock(&mutex);
#endif // PLATFORM_OS_WINDOWS
	}
	return bResult;
}

SimpleThread::SimpleThread()
{
	m_bState	= FALSE;
	m_pFunction = NULL;
	m_pParam	= NULL;
	m_uThreadId = 0;

#ifdef PLATFORM_OS_WINDOWS
	m_hThread = NULL;
#endif //PLATFORM_OS_WINDOWS
}

SimpleThread::~SimpleThread()
{
	ASSERT(!m_uThreadId);
}

BOOL SimpleThread::Start(ThreadFun pFunction, VOID* pParam)
{
	printf(">>>>>>SimpleThread::Start\n");
	ASSERT(!m_uThreadId && pFunction);
	m_pFunction = pFunction;
	m_pParam = pParam;
	BOOL bReFalseCode = FALSE;
#ifdef PLATFORM_OS_WINDOWS
	m_hThread = (HANDLE)::_beginthreadex(0, 0, &SimpleThread::ThreadFunction, (VOID*)this, 0, &m_uThreadId);
	bReFalseCode = (m_hThread == NULL);
#else
	bReFalseCode = ::pthread_create(&m_uThreadId, NULL, &SimpleThread::ThreadFunction, this);
#endif //PLATFORM_OS_WINDOWS
	if (bReFalseCode)
	{
		ASSERT(FALSE);
		m_pFunction = NULL;
		m_pParam = NULL;
		return FALSE;
	}
	m_bState = TRUE;
	return TRUE;
}

VOID SimpleThread::Stop()
{
	if (!m_bState)
	{
		return;
	}
#ifdef PLATFORM_OS_WINDOWS 
	ASSERT(m_uThreadId);
	DWORD dwResult = ::WaitForSingleObject(m_hThread, 60 * 1000);
	if (dwResult == WAIT_TIMEOUT)
	{
		::TerminateThread(m_hThread, (DWORD)(-1));
	}
	::CloseHandle(m_hThread);
    m_uThreadId = NULL;
#else	 
	ASSERT(m_uThreadId > 0);
	void* pRet = NULL;
	::pthread_join(m_uThreadId, &pRet);
    m_uThreadId = 0;
#endif //PLATFORM_OS_WINDOWS	   
	m_pFunction = NULL;
	m_pParam = NULL;
	m_bState = FALSE;
}

#ifdef PLATFORM_OS_WINDOWS
THREAD_FUNC_RET_TYPE SimpleThread::ThreadFunction(VOID* pParam)
#else
THREAD_FUNC_RET_TYPE SimpleThread::ThreadFunction(VOID* pParam)
#endif //PLATFORM_OS_WINDOWS
{
	printf("<<<<<<<<<<<<<<SimpleThread::ThreadFunction(VOID* pParam)\n");
	SimpleThread* pThread = (SimpleThread*)pParam;
	(pThread->m_pFunction)(pThread->m_pParam);
	return 0;
}
