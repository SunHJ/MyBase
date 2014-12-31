#ifndef __COMMON_UNIVERSAL_QUEUE_H__
#define __COMMON_UNIVERSAL_QUEUE_H__

#include "Public.h"

#include "CriticalSection.h"
#include "Semaphore.h"

#include <list>
#include <map>

template <class T>
class UniversalQueue : private UnCopyable
{
private:
    typedef std::list<T> ElementQueue;

private:
    DWORD            m_dwQueueID;                      // the id of the universal queue
    ElementQueue     m_elementQueue;                   // the real queue
    Semaphore        m_semaphore;                      // the manually reset event
    CriticalSection  m_cs;                             // the critical section

public:
    UniversalQueue() : m_dwQueueID(0)
    {
    }

    virtual ~UniversalQueue()
    {
    }

public:
    BOOL  SetQueueID(CONST DWORD dwQueueID)
    {
        ASSERT(0 != dwQueueID);
        m_dwQueueID = dwQueueID;
        return TRUE;
    }

    DWORD GetQueueID() CONST
    {
        return m_dwQueueID;
    }

    BOOL  PushNode(CONST T &node)
    {
        Guard<CriticalSection> guard(m_cs);
        m_elementQueue.push_back(node);
        m_semaphore.ReleaseSemaphore();
        
		return TRUE;
    }

    BOOL  PopNode (T &node)
    {
        if(!m_semaphore.WaitSemaphore())
        {
            return FALSE;
        }

        Guard<CriticalSection> guard(m_cs);
        node = *(m_elementQueue.begin());
        m_elementQueue.pop_front();

        return TRUE;
    }

    T   PopNode()
    {
        m_semaphore.WaitSemaphore();
        Guard<CriticalSection> guard(m_cs);
        T node = *(m_elementQueue.begin());     // copy the node then delete it.
        m_elementQueue.pop_front();

        return node;
    }
};

//通用队列管理类
template <class T>
class UniversalQueueManagement : private UnCopyable
{
    DECLARE_SINGLETON_PATTERN( UniversalQueueManagement );

private:
    typedef UniversalQueue<T>               UnQueueInstance;
    typedef UnQueueInstance *               PUniversalQueue;
	//构造通用队列智能指针
    typedef SharedPtr<UnQueueInstance>      SPUniversalQueue;

    typedef std::map<DWORD, SPUniversalQueue> UniversalQueueMap;

private:
    UniversalQueueMap m_universalQueueMap;

public:
    UniversalQueueManagement()
    {
        m_universalQueueMap.clear();
    }

    ~UniversalQueueManagement()
    {
        m_universalQueueMap.clear();
    }

public:
    SPUniversalQueue GetUniversalQueue(DWORD dwQueueID)
    {
        BOOL bRetCode = FALSE;

        typename UniversalQueueMap::iterator iter = m_universalQueueMap.find(dwQueueID);
        if (m_universalQueueMap.end() != iter)
        {
            return iter->second;
        }

        SPUniversalQueue spUniversalQueue(::new UnQueueInstance());
        bRetCode = spUniversalQueue->SetQueueID(dwQueueID);
        AddUniversalQueue(dwQueueID, spUniversalQueue);

        return spUniversalQueue;
    }

    BOOL AddUniversalQueue(DWORD dwQueueID, SPUniversalQueue spUniversalQueue)
    {
        CHECK_RETURN_BOOL(spUniversalQueue);
        m_universalQueueMap[dwQueueID] = spUniversalQueue;
        return TRUE;
    }
};

#endif //__COMMON_UNIVERSAL_QUEUE_H__
