#include "ThreadPool.h"

ThreadPool::ThreadPool(DWORD dwNum /*= 4*/) : _lThreadNum(0), _lRunningNum(0)
{
    _EventComplete = CreateEvent(0, FALSE, FALSE, NULL);
    _EventEnd = CreateEvent(0, FALSE, FALSE, NULL);
    _SemaphoreCall = CreateSemaphore(0, 0, 0x7FFFFFFF, NULL);
    _SemaphoreDel = CreateSemaphore(0, 0, 0x7FFFFFFF, NULL);
    assert(_SemaphoreCall != INVALID_HANDLE_VALUE);
    assert(_SemaphoreDel != INVALID_HANDLE_VALUE);
    assert(_EventComplete != INVALID_HANDLE_VALUE);
    assert(_EventEnd != INVALID_HANDLE_VALUE);
    AdjustPoolSize(dwNum <= 0 ? 4 : dwNum);
}

ThreadPool::~ThreadPool()
{
    CloseHandle(_EventEnd);
    CloseHandle(_EventComplete);
    CloseHandle(_SemaphoreCall);
    CloseHandle(_SemaphoreDel);
    threadVector.Clear(gSafeDeletePtr);
}

int ThreadPool::AdjustPoolSize(int iNum)
{
    if (iNum > 0)
    {
        ThreadItem* pNew = NULL;
        for (int _i = 0; _i < iNum; _i++)
        {
            pNew = new ThreadItem(this);
            assert(pNew);

            pNew->_Handle = CreateThread(NULL, 0, DefaultJobProc, pNew, 0, NULL);
            assert(pNew->_Handle);

            threadVector.Push(pNew);
            SetThreadPriority(pNew->_Handle, THREAD_PRIORITY_NORMAL);
        }
    }
    else
    {
        iNum *= -1;
        ReleaseSemaphore(_SemaphoreDel, iNum > _lThreadNum ? _lThreadNum : iNum, NULL);
    }
    return (int)_lThreadNum;
}

DWORD ThreadPool::DefaultJobProc(LPVOID lpParameter /* = NULL */)
{
    ThreadItem* pThread = static_cast<ThreadItem*>(lpParameter);
    assert(pThread);
    ThreadPool* pThreadPoolObj = pThread->_pThis;
    assert(pThreadPoolObj);
    InterlockedIncrement(&pThreadPoolObj->_lThreadNum);
    HANDLE hWaitHandle[3];                     
    hWaitHandle[0] = pThreadPoolObj->_EventEnd;
    hWaitHandle[1] = pThreadPoolObj->_SemaphoreDel;
    hWaitHandle[2] = pThreadPoolObj->_SemaphoreCall;

    bool fHasJob = false;

    for (;;)
    {
        DWORD wr = WaitForMultipleObjects(3, hWaitHandle, false, INFINITE);
        if (wr == WAIT_OBJECT_0 || wr == WAIT_OBJECT_0 + 1)
            break;

        if (wr == WAIT_OBJECT_0 + 2)
        {
            JobItem* pJob = pThreadPoolObj->jobQueue.Pop();
            if (pJob)
            {
                InterlockedIncrement(&pThreadPoolObj->_lRunningNum);
                pThread->_dwLastBeginTime = GetTickCount();
                pThread->_dwCount++;
                pThread->_fIsRunning = true;
                pJob->_pFunc(pJob->_pParam); //运行用户作业
                delete pJob; pJob = NULL;
                pThread->_fIsRunning = false;
                InterlockedDecrement(&pThreadPoolObj->_lRunningNum);
            }
        } 
    }
    pThreadPoolObj->threadVector.Earse(pThread);
    delete pThread; pThread = NULL;

    InterlockedDecrement(&pThreadPoolObj->_lThreadNum);
    
    if (!pThreadPoolObj->_lThreadNum)
        SetEvent(pThreadPoolObj->_EventComplete);
    return 0;
}

void ThreadPool::Call(ThreadJob* pJob, void* pJobParam /* = NULL */)
{
    CallProcParam* pNewProcParam = new CallProcParam(pJob, pJobParam);
    assert(pNewProcParam);
    JobItem* pNewJob = new JobItem(CallProc, pNewProcParam);
    assert(pNewJob);

    jobQueue.Push(pNewJob);
    ReleaseSemaphore(_SemaphoreCall, 1, NULL);
}

bool ThreadPool::IsRunning()
{
    return _lRunningNum > 0;
}

bool ThreadPool::EndAndWait(DWORD dwWaitTime /* = INFINITE */)
{
    while (_lThreadNum)
    {
        EndAThread();
    }
    return WaitForSingleObject(_EventComplete, dwWaitTime) == WAIT_OBJECT_0;
}

void ThreadPool::CallProc(void* pParam)
{
    CallProcParam* cp = static_cast<CallProcParam*>(pParam);
    assert(cp);
    if (cp)
    {
        cp->_pObj->DoJob(cp->_pParam);
        delete cp;
    }
}
