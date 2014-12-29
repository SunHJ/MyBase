#ifndef __COMMON_REFCOUNTER_H__
#define __COMMON_REFCOUNTER_H__

#include <memory>
#include "Global.h"

class RefCounterBase : private UnCopyable
{
private:
	// The strong-counter use weak-reference-counter once.
	LONG m_lStrongRefCounter;   // control the life-cycle of res 
	LONG m_lWeakRefCounter;     // control the life cycle of BRefCounter and res-pointer

public:
	RefCounterBase() : m_lStrongRefCounter(1), m_lWeakRefCounter(1)
	{
	}

	virtual ~RefCounterBase()
	{
	}

private:
	virtual VOID DestroyRes() = 0;   // derived class realizes how to release resources.
	virtual VOID DestroySelf()       // destroy BRefCounter instance
	{
		g_DeleteConstPtr(this);
	}

public:
	VOID DelStrongRef()
	{
		if (0 == g_AtomicDecrement(&m_lStrongRefCounter))
		{
			DestroyRes();
			DelWeakRef();
		}
	}

	VOID DelWeakRef()
	{
		if (0 == g_AtomicDecrement(&m_lWeakRefCounter))
		{
			DestroySelf();
		}
	}

	VOID AddStrongRef()
	{
		LONG lRetValue = g_AtomicIncrement(&m_lStrongRefCounter);
		ASSERT(lRetValue > 0);
	}

	VOID AddWeakRef()
	{
		LONG lRetValue = g_AtomicIncrement(&m_lWeakRefCounter);
		ASSERT(lRetValue > 0);
	}

	LONG GetStrongRefCounter() CONST
	{
		return static_cast<CONST LONG volatile &>(m_lStrongRefCounter);
	}

	LONG GetWeakRefCounter() CONST
	{
		return static_cast<CONST LONG volatile &>(m_lWeakRefCounter);
	}

	virtual PVOID GetDeleter() = 0;
};

template <class T>
class BRefCounter : public RefCounterBase
{
private:
	typedef VOID (*Dtr)(T *&p);
	typedef BRefCounter<T> this_type;

private:
	T *m_ptr;       // ptr to the real res
	Dtr m_deteler;  // deleter

public:
	BRefCounter() : m_ptr(NULL), m_deteler(g_SafelyDeletePtr<T>)
	{
	}

	explicit BRefCounter(T *ptr) : m_ptr(ptr), m_deteler(g_SafelyDeletePtr<T>)
	{
	}

	BRefCounter(T *ptr, Dtr dtr) : m_ptr(ptr), m_deteler(dtr)
	{
	}

	virtual ~BRefCounter()
	{
	}

public:
	virtual PVOID GetDeleter()
	{
		return static_cast<PVOID>(m_deteler);
	}

private:
	virtual VOID DestroyRes()
	{
		m_deteler(m_ptr);
	}
};

class WeakCounter;
class StrongCounter
{
private:
	friend class WeakCounter;

private:
	RefCounterBase *m_pCounter;

public:
	StrongCounter() : m_pCounter(NULL)
	{
	}

	template <class D>
	explicit StrongCounter(D *ptr) : m_pCounter(NULL)
	{
		m_pCounter = ::new BRefCounter<D>(ptr);
		ASSERT(NULL != m_pCounter);
	}

	template <class D, class Dtr>
	StrongCounter(D *ptr, Dtr dtr) : m_pCounter(NULL)
	{
		m_pCounter = ::new BRefCounter<D>(ptr, dtr);
		ASSERT(NULL != m_pCounter);
	}

	template <class D>
	explicit StrongCounter(std::auto_ptr<D> &autoPtr)
	{
		m_pCounter = ::new BRefCounter<T>(autoPtr.get());
		ASSERT(NULL != m_pCounter);
		autoPtr.release();
	}

	StrongCounter(CONST StrongCounter &r) : m_pCounter(r.m_pCounter)
	{
		if(NULL != m_pCounter)
		{
			m_pCounter->AddStrongRef();
		}
	}

	explicit StrongCounter(CONST WeakCounter &r);

	~StrongCounter()
	{
		if(NULL != m_pCounter)
		{
			m_pCounter->DelStrongRef();
		}
	}

	StrongCounter (StrongCounter &&r) : m_pCounter(r.m_pCounter)
	{
		r.m_pCounter = NULL;
	}

	StrongCounter &operator=(CONST StrongCounter &rhs)
	{
		RefCounterBase *pTmp = rhs.m_pCounter;

		if(pTmp != m_pCounter)
		{ // check if is self
			if(NULL != pTmp) pTmp->AddStrongRef();
			if(NULL != m_pCounter) m_pCounter->DelStrongRef();
			m_pCounter = pTmp;
		}

		return *this;
	}

	VOID Swap(StrongCounter &r)
	{
		g_Swap(m_pCounter, r.m_pCounter);
	}

	LONG GetStrongRefCounter() CONST
	{
		return m_pCounter != NULL ? m_pCounter->GetStrongRefCounter() : 0;
	}

	LONG GetWeakRefCounter() CONST
	{
		return m_pCounter != NULL ? m_pCounter->GetWeakRefCounter() : 0;
	}

	BOOL IsUnique() CONST
	{
		return GetStrongRefCounter() == 1;
	}

	BOOL IsEmpty() CONST
	{
		return m_pCounter == NULL;
	}

	friend inline bool operator==(CONST StrongCounter &lhs, CONST StrongCounter &rhs)
	{
		return lhs.m_pCounter == rhs.m_pCounter;
	}

	friend inline bool operator!=(CONST StrongCounter &lhs, CONST StrongCounter &rhs)
	{
		return lhs.m_pCounter != rhs.m_pCounter;
	}
};

class WeakCounter
{
private:
	friend class StrongCounter;

private:
	RefCounterBase  *m_pCounter;

public:
	WeakCounter() : m_pCounter(NULL)
	{
	}

	WeakCounter(CONST WeakCounter &r) : m_pCounter(r.m_pCounter)
	{
		if(NULL != m_pCounter)
		{
			m_pCounter->AddWeakRef();
		}
	}

	WeakCounter(CONST StrongCounter &r) : m_pCounter(r.m_pCounter)
	{
		if(NULL != m_pCounter)
		{
			m_pCounter->AddWeakRef();
		}
	}

	WeakCounter(WeakCounter &&r) : m_pCounter(r.m_pCounter)
	{
		m_pCounter = NULL;
	}

	WeakCounter &operator=(CONST WeakCounter &rhs)
	{
		RefCounterBase *pTmp = rhs.m_pCounter;
		if(NULL != pTmp) pTmp->AddWeakRef();
		if(NULL != m_pCounter) m_pCounter->DelWeakRef();
		m_pCounter = pTmp;
		return *this;
	}

	WeakCounter &operator=(CONST StrongCounter &rhs)
	{
		RefCounterBase *pTmp = rhs.m_pCounter;
		if(NULL != pTmp) pTmp->AddWeakRef();
		if(NULL != m_pCounter) m_pCounter->DelWeakRef();
		m_pCounter = pTmp;
		return *this;
	}

	~WeakCounter()
	{
		if(NULL != m_pCounter)
		{
			m_pCounter->DelWeakRef();
		}
	}

	VOID Swap(WeakCounter &r)
	{   
		g_Swap(m_pCounter, r.m_pCounter);
	}

	LONG GetStrongRefCounter() CONST
	{
		return m_pCounter != 0 ? m_pCounter->GetStrongRefCounter() : 0;
	}

	LONG GetWeakRefCounter() CONST
	{
		return m_pCounter != 0 ? m_pCounter->GetWeakRefCounter() : 0;
	}

	BOOL IsEmpty() CONST
	{
		return m_pCounter == 0;
	}

	friend inline bool operator==(CONST WeakCounter &lhs, CONST WeakCounter &rhs)
	{
		return lhs.m_pCounter == rhs.m_pCounter;
	}

	friend inline bool operator!=(CONST WeakCounter &lhs, CONST WeakCounter &rhs)
	{
		return lhs.m_pCounter != rhs.m_pCounter;
	}
};

inline StrongCounter::StrongCounter(CONST WeakCounter &r) : m_pCounter(r.m_pCounter)
{
	if (NULL != m_pCounter && 0 != m_pCounter->GetStrongRefCounter())
	{ // TODO : here is not thread safe
		m_pCounter->AddStrongRef();
	}
	else
		m_pCounter = NULL;
}

#endif //__COMMON_REFCOUNTER_H__