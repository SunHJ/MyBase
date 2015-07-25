#pragma once
#pragma warning(disable:4530)
#pragma warning(disable:4786)

#include "PublicStruct.h"

typedef void(*ThreadFun)(void *);

class ThreadJob
{
public:
    virtual void DoJob(void* pParam) = 0;
};

struct CallProcParam
{
	ThreadJob* _pObj;
	void* _pParam;
	CallProcParam(ThreadJob* p, void* pParam) : _pObj(p), _pParam(pParam) {};
};

struct JobItem
{
	ThreadFun _pFunc;
	void* _pParam;
	JobItem(ThreadFun pFun = NULL, void* pParam = NULL) :_pFunc(pFun), _pParam(pParam){};
};


class ThreadPool
{
    struct ThreadItem
    {
        HANDLE      _Handle; // 线程句柄
        ThreadPool* _pThis; // 线程池指针
        DWORD       _dwLastBeginTime; // 最后一次运行时间
        DWORD       _dwCount; // 运行次数
        bool        _fIsRunning; // 现成当前状态
        ThreadItem(ThreadPool* pThis) : _pThis(pThis), _Handle(INVALID_HANDLE_VALUE),
            _dwLastBeginTime(0), _dwCount(0), _fIsRunning(false) {};
        ~ThreadItem()
        {
            if (_Handle != INVALID_HANDLE_VALUE)
            {
                ::CloseHandle(_Handle);
                _Handle = INVALID_HANDLE_VALUE;
            }
        }
    };
public:
    ThreadPool(DWORD dwNum = 4);

    ~ThreadPool();

    int AdjustPoolSize(int iNum);

    bool IsRunning();

    bool EndAndWait(DWORD dwWaitTime = INFINITE);

    void Call(ThreadJob* pJob, void* pJobParam = NULL);

	inline void EndAThread() { SetEvent(_EventEnd); };

	inline DWORD Size() const { return (DWORD)_lThreadNum; };

	inline DWORD GetRunningSize() const { return (DWORD)_lRunningNum; };

protected:
    static void CallProc(void* pParam);

    static DWORD WINAPI DefaultJobProc(LPVOID lpParameter = NULL);

private:
	MTQueue<JobItem> jobQueue; // 工作队列
    MTVector<ThreadItem> threadVector; // 线程数据

	long _lThreadNum; // 线程池的线程数
	long _lRunningNum; // 当前运行的线程数
	HANDLE _EventEnd; // 某一线程结束事件
	HANDLE _SemaphoreDel; // 删除线程信号
	HANDLE _SemaphoreCall; // 任务到达信号
	HANDLE _EventComplete; // 线程池完成事件
};