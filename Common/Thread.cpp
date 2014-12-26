#include "Macro.h"
#include "Global.h"
#include "Thread.h"

Thread::Thread()
{
#ifdef WIN32
		m_hThread = NULL;
#else
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&cond, NULL);
#endif
    m_eFlag = eNone;
    m_threadID = 0;
}

Thread::~Thread()
{
	Thread::Stop();
}

THREAD_ID Thread::GetThreadID() CONST
{
	return m_threadID;
}

BOOL Thread::Init()
{
	return TRUE;
}

#ifdef WIN32

HANDLE Thread::GetHandle() CONST
{
	return m_hThread;
}

BOOL Thread::Wait(DWORD dwTimeOutMillis /* = 0 */) CONST
{
	CHECK_RETURN_BOOL_QUIET(m_threadID > 0);
	CHECK_RETURN_BOOL_QUIET(m_hThread != INVALID_HANDLE_VALUE);
	DWORD dwRetCode = ::WaitForSingleObject(m_hThread, dwTimeOutMillis);
	return WAIT_OBJECT_0 == dwRetCode;
}

THREAD_FUNC_RET_TYPE __stdcall Thread::ThreadFunction(VOID *pValue)
{
    UINT                 uReCode     = 0;
    THREAD_FUNC_RET_TYPE reValue     = 0;
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

BOOL Thread::Start()
{
    BOOL bResult = FALSE;
	if (m_threadID <= 0 || INVALID_HANDLE_VALUE == m_hThread)
	{
		m_hThread = (HANDLE)::_beginthreadex(0, 0, ThreadFunction, (VOID*)this, 0, &m_threadID);
        if (m_hThread > 0)
        {
            m_eFlag = eActive;
            bResult = TRUE;
        }
	}
	return bResult;
}

VOID Thread:PreRun()
{
}

BOOL Thread::Stop(DWORD dwExitCode /* = 0 */)
{
	CHECK_RETURN_BOOL_QUIET(m_threadID > 0);
	CHECK_RETURN_BOOL_QUIET(m_hThread != INVALID_HANDLE_VALUE);
	BOOL bRetCode = ::TerminateThread(m_hThread, dwExitCode);
	if (bRetCode)
	{
		g_CloseHandle(m_hThread);
		return TRUE;
	}
	return FALSE;
}

BOOL Thread::SuspendThread()
{
	CHECK_RETURN_BOOL_QUIET(m_threadID > 0);

	CHECK_RETURN_BOOL_QUIET(m_hThread != INVALID_HANDLE_VALUE);
	DWORD dwRetCode = ::SuspendThread(m_hThread);
	if (static_cast<DWORD>(-1) == dwRetCode)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL Thread::ResumeThread()
{
	CHECK_RETURN_BOOL_QUIET(m_threadID > 0);
	CHECK_RETURN_BOOL_QUIET(m_hThread != INVALID_HANDLE_VALUE);
	DWORD dwRetCode = ::ResumeThread(m_hThread);
	if (static_cast<DWORD>(-1) == dwRetCode)
	{
		return FALSE;
	}

	return TRUE;
}

#else

THREAD_FUNC_RET_TYPE Thread::ThreadFunction(VOID *pValue)
{
    UINT                 uReCode     = 0;
    THREAD_FUNC_RET_TYPE reValue     = 0;
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

BOOL Thread::Start()
{
    BOOL bResult = FALSE;
	if (m_threadID <= 0)
	{
		INT iRetCode = ::pthread_create(&m_threadID, NULL, ThreadFunction, (VOID*)this);
        if (iRetCode == 0)
        {
            m_eFlag = eActive;
            bResult = TRUE;
        }
	}
	return bResult;
}

VOID Thread::PreRun()
{
    pthread_mutex_lock(&mutex);
    while(m_eFlag == eSuspend)
    {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
}

BOOL Thread::Stop(DWORD dwExitCode /* = 0 */)
{
	CHECK_RETURN_BOOL_QUIET(m_threadID>0);
	int iRetCode = ::pthread_cancel(m_threadID);
	return (iRetCode == 0);
}

BOOL Thread::SuspendThread()
{
	CHECK_RETURN_BOOL_QUIET(m_threadID > 0);
    BOOL bResult = FALSE;
    pthread_mutex_lock(&mutex);
    if (m_eFlag == eActive)
    {
        m_eFlag = eSuspend;
        bResult = TRUE;
    }
    pthread_mutex_unlock(&mutex);
	return bResult;
}

BOOL Thread::ResumeThread()
{
	CHECK_RETURN_BOOL_QUIET(m_threadID > 0);
    BOOL bResult = FALSE;
    pthread_mutex_lock(&mutex);
    if (m_eFlag == eSuspend)
    {
        m_eFlag = eActive;
        pthread_cond_signal(&cond);
        bResult = TRUE;
    }
    pthread_mutex_unlock(&mutex);
	return bResult;
}

#endif
