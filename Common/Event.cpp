
#include "Event.h"
#include "Global.h"

inline VOID g_DestroyEvent(HANDLE_EVENT &hEvent)
{
	if (NULL != hEvent)
	{
#ifdef WIN32
	g_CloseHandle(hEvent);
#else
	::pthread_cond_destory(&hEvent->m_cond);
	g_SafelyDeletePtr(hEvent);
#endif // WIN32
	}
}

inline HANDLE_EVENT g_CreateEvent(BOOL bManualReset, BOOL bInitialState)
{
#ifdef PLATFORM_OS_WINDOWS
	HANDLE_EVENT hEvent = ::CreateEvent(NULL, bManualReset, bInitialState, NULL);
	PROCESS_ERROR(NULL != hEvent);
#else
	INT nReCode = 0;
	// std::nothrow 在内存不足时，new (std::nothrow)并不抛出异常，而是将指针置NULL。
	HANDLE_EVENT hEvent = ::new(std::nothrow) EventHandleData;
	PROCESS_ERROR(NULL != hEvent);

	hEvent->m_bState = bInitialState;
	hEvent->m_bManualReset = bManualReset;
	nReCode = ::pthread_cond_init(&hEvent->m_cond, NULL);
	PROCESS_ERROR(nReCode);
#endif // PLATFORM_OS_WINDOWS
	return hEvent;

Exit0:
	g_DestroyEvent(hEvent);
	return NULL;
}

#ifdef PLATFORM_OS_LINUX
typedef struct timespec AbsTimeSpec;
inline AbsTimeSpec& g_GetAbsTime(DWORD dwMilliseconds)
{
	ASSERT(INFINITE != dwMilliseconds);
	static AbsTimeSpec abstime;
	struct timeval tv;
	::memset(&abstime, 0, sizeof(AbsTimeSpec));
	::memset(&tv, 0, sizeof(struct timeval));

	::gettimeofday(&tv, NULL);
	abstime.tv_sec = tv.tv_sec + dwMilliseconds / 1000;
	abstime.tv_nsec = tv.tv_usec * 100 + (dwMilliseconds % 1000) * 1000000;

	if (abstime.tv_nsec >= 1000000000)
	{
		abstime.tv_nsec -= 1000000000;
		abstime.tv_sec++;
	}
	return abstime;
}

#endif // PLATFORM_OS_LINUX


inline BOOL g_WaitEvent(HANDLE_EVENT hEvent, DWORD dwMilliseconds /* = INFINITE */)
{
#ifdef PLATFORM_OS_WINDOWS
	CHECK_RETURN_BOOL_QUIET(NULL!=hEvent && INVALID_HANDLE_VALUE!=hEvent);
	DWORD dwRet = ::WaitForSingleObject(hEvent, dwMilliseconds);
	return (WAIT_OBJECT_0 == dwRet);
#else
	CHECK_RETURN_BOOL_QUIET(NULL!=hEvent); 
	Guard<CriticalSection> guard(hEvent->m_cs);

	INT nReCode = 0;
	AbsTimeSpec abstime;
	if (INFINITE != dwMilliseconds)
	{
		abstime = g_GetAbsTime(dwMilliseconds);
	}

	while (!hEvent->m_bState)
	{
		Mutex &cs = hEvent->m_cs.GetCS();
		if (INFINITE == dwMilliseconds)
		{
			nReCode = ::pthread_cond_wait(&hEvent->m_cond, &cs)
		}
		else
		{
			nReCode = ::pthread_cond_timedwait(&hEvent->m_cond, &cs, &abstime)
		}
		CHECK_RETURN_BOOL_QUIET(0 == nReCode);

		if (!hEvent->m_bManualReset)
		{
			hEvent->m_bState = FALSE;
		}

		return TRUE;
	}
#endif // PLATFORM_OS_WINDOWS
}

inline BOOL g_SetEvent(HANDLE_EVENT hEvent)
{
	CHECK_RETURN_BOOL_QUIET(NULL!=hEvent);
#ifdef WIN32
	CHECK_RETURN_BOOL_QUIET(INVALID_HANDLE_VALUE!=hEvent);
	return ::SetEvent(hEvent);

#else
	INT nReCode = 0;
	Guard<CriticalSection> guard(hEvent->m_cs);
	hEvent->m_bState = TRUE;

	if (hEvent->m_bManualReset)
	{
		nReCode = ::pthread_cond_broadcast(&hEvent->m_cond);
	}
	else
	{
		nReCode = ::pthread_cond_signal(&hEvent->m_cond);
	}
	CHECK_RETURN_BOOL_QUIET(0 == nReCode);

	return TRUE;
#endif // WIN32
}

inline BOOL g_ResetEvent(HANDLE_EVENT hEvent)
{
	CHECK_RETURN_BOOL_QUIET(NULL != hEvent);
#ifdef WIN32
	CHECK_RETURN_BOOL_QUIET(INVALID_HANDLE_VALUE != hEvent);
	return ::ResetEvent(hEvent);
#else
	Guard<CriticalSection> guard(hEvent->m_cs);
	hEvent->m_bState = FALSE;
	return TRUE;
#endif
}

Event::Event(BOOL bManualReset, BOOL bInitialState)
{
	m_hEvent = g_CreateEvent(bManualReset, bInitialState);
	ASSERT(m_hEvent != NULL);
}

Event::~Event()
{
	g_DestroyEvent(m_hEvent);
}

BOOL Event::WaitEvent(DWORD dwMilliseconds /* = INFINITE */)
{
	return g_WaitEvent(m_hEvent, dwMilliseconds);
}

BOOL Event::SetEvent()
{
	return g_SetEvent(m_hEvent);
}

BOOL Event::ResetEvent()
{
	return g_ResetEvent(m_hEvent);
}

ManualResetEvent::ManualResetEvent(BOOL bInitialState /* = FALSE */) : Event(TRUE, bInitialState)
{
	//
}

ManualResetEvent::~ManualResetEvent()
{
	//
}

AutoResetEvent::AutoResetEvent(BOOL bInitialState /* = FALSE */) : Event(FALSE, bInitialState)
{
	//
}

AutoResetEvent::~AutoResetEvent()
{
	//
}