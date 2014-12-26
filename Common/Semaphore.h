#ifndef __SYNCHRONIZATION_SEMAPHORE_H__
#define __SYNCHRONIZATION_SEMAPHORE_H__

#include "Type.h"

class Semaphore : private UnCopyable 
{
private:
	static CONST LONG SEMAPHORE_MAX_NUM = 1024; 

private:
	HANDLE m_hSemaphore;

public:
	Semaphore(LONG nInitNum = 0, LONG nMaxNum = SEMAPHORE_MAX_NUM);
	virtual ~Semaphore();

public:
	BOOL ReleaseSemaphore(LONG nReleaseNum = 1);
	BOOL WaitSemaphore(DWORD dwMilliseconds = INFINITE);
};

#endif //__SYNCHRONIZATION_SEMAPHORE_H__