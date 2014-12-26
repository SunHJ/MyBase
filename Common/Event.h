#ifndef __SYNCHRONIZATION_EVENT_H__
#define __SYNCHRONIZATION_EVENT_H__

#include "Type.h"

#ifdef PLATFORM_OS_WINDOWS
    typedef HANDLE HANDLE_EVENT;
#else
    typedef struct {
        BOOL m_bState;
        BOOL m_bManualReset;
        CriticalSection m_cs;
        pthread_cond_t  m_cond;
    } EventHandleData;
    typedef  EventHandleData* HANDLE_EVENT;
#endif

class Event : private UnCopyable
{
private:
    HANDLE_EVENT m_hEvent;

    Event();

public:
    Event(BOOL bManualReset, BOOL bInitialState);
    virtual ~Event();

    BOOL WaitEvent(DWORD dwMilliseconds = INFINITE);
    BOOL SetEvent();
    BOOL ResetEvent();
};

class ManualResetEvent : public Event
{
public:
    explicit ManualResetEvent(BOOL bInitialState = FALSE);
    virtual ~ManualResetEvent();
};

class AutoResetEvent : public Event
{
public:
    explicit AutoResetEvent(BOOL bInitialState = FALSE);
    virtual ~AutoResetEvent();
};

#endif //__SYNCHRONIZATION_EVENT_H__