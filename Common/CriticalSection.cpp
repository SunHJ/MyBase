#include "CriticalSection.h"

CriticalSection::CriticalSection()
{
#ifdef PLATFORM_OS_WINDOWS
	::InitializeCriticalSection(&m_cs);
#else
	::pthread_mutex_init(&m_cs, NULL);
#endif
}

CriticalSection::~CriticalSection()
{
#ifdef PLATFORM_OS_WINDOWS
	::DeleteCriticalSection(&m_cs);
#else
	::pthred_mutex_destory(&m_cs);
#endif
}

Mutex &CriticalSection::GetCS()
{
	return m_cs;
}

VOID CriticalSection::Lock()
{
#ifdef PLATFORM_OS_WINDOWS
	::EnterCriticalSection(&m_cs);
#else
	::pthread_mutex_lock(&m_cs);
#endif
}

VOID CriticalSection::UnLock()
{
#ifdef PLATFORM_OS_WINDOWS
	::LeaveCriticalSection(&m_cs);
#else
	::pthread_mutex_unlock(&m_cs);
#endif
}

