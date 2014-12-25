#include "Macro.h"
#include "Global.h"
#include "Thread.h"

Thread::Thread()
{
#ifdef WIN32
		m_hThread = NULL;
		m_threadID = 0;
#else
		m_threadID = 0;
#endif
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
	Thread* pThisThread = (Thread*)pValue;
	return (THREAD_FUNC_RET_TYPE)(pThisThread ? pThisThread->Run() : 0);
}

BOOL Thread::Start()
{
	if (m_threadID <= 0 || INVALID_HANDLE_VALUE == m_hThread)
	{
		m_hThread = (HANDLE)::_beginthreadex(0, 0, ThreadFunction, (VOID*)this, 0, &m_threadID);
	}
	return (m_threadID > 0);
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
	Thread* pThisThread = (Thread*)pValue;
	return (THREAD_FUNC_RET_TYPE)(pThisThread ? pThisThread->Run() : 0);
}

BOOL Thread::Start()
{
	int iRetCode = 0;
	if (m_threadID <= 0)
	{
		iRetCode = ::pthread_create(&m_threadID, NULL, ThreadFunction, (VOID*)this);
	}
	return (iRetCode == 0);

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

	return TRUE;
}

BOOL Thread::ResumeThread()
{
	CHECK_RETURN_BOOL_QUIET(m_threadID > 0);

	return TRUE;
}
#endif
