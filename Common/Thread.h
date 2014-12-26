#ifndef __SYNCHRONIZATION_THREAD_H__
#define __SYNCHRONIZATION_THREAD_H__

#include "Type.h"

#ifdef WIN32
	typedef UINT THREAD_ID;
	typedef UINT THREAD_FUNC_RET_TYPE;
#else
	typedef pthread_t THREAD_ID;
	typedef VOID * THREAD_FUNC_RET_TYPE;
#endif

typedef enum _ThreadStatus{
    eNone,
    eActive,
    eSuspend
}emThreadStatus;

class Thread : private UnCopyable
{
private:
	emThreadStatus  m_eFlag;
	THREAD_ID       m_threadID;

#ifdef WIN32
	HANDLE m_hThread;
	HANDLE GetHandle() CONST;
	static THREAD_FUNC_RET_TYPE __stdcall ThreadFunction(VOID *);
#else
    pthread_mutex_t mutex;
    pthread_cond_t cond;
	static THREAD_FUNC_RET_TYPE ThreadFunction(VOID *);
#endif

protected:
	Thread();
	virtual ~Thread();

public:
	virtual BOOL Init();

	THREAD_ID GetThreadID() CONST;
	virtual BOOL Start();
	virtual BOOL Stop(DWORD dwExitCode = 0);
	virtual BOOL SuspendThread();
	virtual BOOL ResumeThread();

private:
    virtual VOID PreRun();
	virtual UINT Run() = 0;
};

#endif	//__SYNCHRONIZATION_THREAD_H__
