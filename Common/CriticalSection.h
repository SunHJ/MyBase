#ifndef __SYNCHRONIZATION_CRITICAL_SECTION_H__
#define __SYNCHRONIZATION_CRITICAL_SECTION_H__
#include "Type.h"

#ifdef PLATFORM_OS_WINDOWS
	typedef CRITICAL_SECTION Mutex;
#else
	typedef pthread_mutex_t Mutex;
#endif // PLATFORM_OS_WINDOWS

class  CriticalSection : private UnCopyable
{
private:
	Mutex m_cs;

public:
	CriticalSection();
	~CriticalSection();

public:
	VOID Lock();
	VOID UnLock();

	Mutex& GetCS();
};

template <class MutexType>
class Guard : private UnCopyable
{
private:
	MutexType &m_mutex;

public:
	explicit Guard(MutexType &mutex) : m_mutex(mutex) { m_mutex.Lock(); }
	~Guard() { m_mutex.UnLock(); }
};

typedef Guard<CriticalSection> MutexGuard;

#endif	//__SYNCHRONIZATION_CRITICAL_SECTION_H__