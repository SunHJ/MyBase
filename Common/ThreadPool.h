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
        HANDLE      _Handle; // �߳̾��
        ThreadPool* _pThis; // �̳߳�ָ��
        DWORD       _dwLastBeginTime; // ���һ������ʱ��
        DWORD       _dwCount; // ���д���
        bool        _fIsRunning; // �ֳɵ�ǰ״̬
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
	MTQueue<JobItem> jobQueue; // ��������
    MTVector<ThreadItem> threadVector; // �߳�����

	long _lThreadNum; // �̳߳ص��߳���
	long _lRunningNum; // ��ǰ���е��߳���
	HANDLE _EventEnd; // ĳһ�߳̽����¼�
	HANDLE _SemaphoreDel; // ɾ���߳��ź�
	HANDLE _SemaphoreCall; // ���񵽴��ź�
	HANDLE _EventComplete; // �̳߳�����¼�
};