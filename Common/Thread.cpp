#include "Macro.h"
#include "Global.h"
#include "Thread.h"

#ifdef PLATFORM_OS_WINDOWS
CONST DWORD INVALID_DW_VALUE = static_cast<DWORD>(-1);
HANDLE Thread::GetHandle() CONST
{
	return m_hThread;
}

THREAD_FUNC_RET_TYPE __stdcall Thread::ThreadFunction(VOID *pValue)
#else
THREAD_FUNC_RET_TYPE Thread::ThreadFunction(VOID *pValue)
#endif // WIN32
{
	UINT                 uReCode = 0;
	THREAD_FUNC_RET_TYPE reValue = 0;
	Thread* pThisThread = (Thread*)pValue;
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

Thread::Thread()
{
	m_eFlag = eNone;
	m_threadID = 0;

#ifdef PLATFORM_OS_WINDOWS
	m_hThread = NULL;
#else
	pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
#endif
}

Thread::~Thread()
{
	Thread::Stop();
}

BOOL Thread::Init()
{
	return TRUE;
}

BOOL Thread::Start()
{
    BOOL bResult = FALSE;
	if (m_threadID <= 0)
	{
#ifdef PLATFORM_OS_WINDOWS
		m_hThread = (HANDLE)::_beginthreadex(0, 0, ThreadFunction, (VOID*)this, 0, &m_threadID);
#else
		::pthread_create(&m_threadID, NULL, ThreadFunction, (VOID*)this);
#endif // PLATFORM_OS_WINDOWS
        if (m_hThread > 0)
        {
            m_eFlag = eActive;
            bResult = TRUE;
        }
	}
	return bResult;
}

VOID Thread::PreRun()
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

BOOL Thread::Stop(DWORD dwExitCode /* = 0 */)
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

BOOL Thread::SuspendThread()
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

BOOL Thread::ResumeThread()
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
