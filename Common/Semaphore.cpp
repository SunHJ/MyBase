#include "Global.h"
#include "Semaphore.h"

Semaphore::Semaphore(LONG nInitNum /* = 0 */, LONG nMaxNum /* = SEMAPHORE_MAX_NUM */)
{
#ifdef PLATFORM_OS_WINDOWS
	m_Sem = ::CreateSemaphore(NULL, nInitNum, nMaxNum, NULL);
	while (!g_CheckHandle(m_Sem))
	{
		ASSERT(FALSE);
	}
#else
	INT nReCode = ::sem_init(&m_Sem, 0, nInitNum);
	if (nReCode == -1)
	{
		ASSERT(FALSE);
	}
#endif // PLATFORM_OS_WINDOWS
}

Semaphore::~Semaphore()
{
#ifdef PLATFORM_OS_WINDOWS
	g_CloseHandle(m_Sem);
#else
	::sem_destroy(&m_Sem);
#endif // PLATFORM_OS_WINDOWS

}

BOOL Semaphore::ReleaseSemaphore(LONG nReleaseNum /* = 1 */)
{
#ifdef PLATFORM_OS_WINDOWS
	CHECK_RETURN_BOOL(NULL!=m_Sem && INVALID_HANDLE_VALUE != m_Sem);
	return ::ReleaseSemaphore(m_Sem, nReleaseNum, NULL);
#else
	if (::sem_post(&m_Sem) == 0)
	{
		return TRUE;
	}
	return FALSE;
#endif // PLATFORM_OS_WINDOWS

}

BOOL Semaphore::WaitSemaphore(DWORD dwMilliseconds /* = INFINITE */)
{
#ifdef PLATFORM_OS_WINDOWS
	CHECK_RETURN_BOOL(NULL!=m_Sem && INVALID_HANDLE_VALUE != m_Sem);
	return WAIT_OBJECT_0 == ::WaitForSingleObject(m_Sem, dwMilliseconds);
#else
	INT nReCode = 0;
	if (dwMilliseconds == INFINITE)
	{
		nReCode = ::sem_wait(&m_Sem);
	}
	else
	{
		AbsTimeSpec abstime = g_GetAbsTime(dwMilliseconds);
		nReCode = ::sem_timedwait(&m_Sem£¬ &abstime);
	}
	return (0 == nReCode);
#endif // PLATFORM_OS_WINDOWS

}
