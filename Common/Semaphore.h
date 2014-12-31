#ifndef __SYNCHRONIZATION_SEMAPHORE_H__
#define __SYNCHRONIZATION_SEMAPHORE_H__

#include "Type.h"

#ifdef PLATFORM_OS_WINDOWS
typedef HANDLE SemType;
#else
typedef sem_t SemType;
#endif // PLATFORM_OS_WINDOWS

class Semaphore : private UnCopyable 
{
private:
	static CONST LONG SEMAPHORE_MAX_NUM = 1024; 

private:
	SemType m_Sem;

public:
	Semaphore(LONG nInitNum = 0, LONG nMaxNum = SEMAPHORE_MAX_NUM);
	virtual ~Semaphore();

public:
	BOOL ReleaseSemaphore(LONG nReleaseNum = 1);
	BOOL WaitSemaphore(DWORD dwMilliseconds = INFINITE);
};

#endif //__SYNCHRONIZATION_SEMAPHORE_H__