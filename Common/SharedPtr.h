#ifndef __COMMON_SHARED_PTR_H__
#define __COMMON_SHARED_PTR_H__

#include "RefCounter.h"

template<class T> class WeakPtr;
template<class T> class SharedPtr;

template<class T>
class SharedPtr
{
public:
	typedef T elem_type;
private:
	template <class D> friend class SharedPtr;
	template <class D> friend class WeakPtr;
	typedef SharedPtr<T> this_type;

private:
	T *m_ptr;
	StrongCounter m_counter;

public:
	SharedPtr() : m_ptr(NULL), m_counter()
	{
	}

	// the "explicit" keyword is necessary, or user may mistake to use SharedPtr like this:
	// SharedPtr<INT> sp = ::new INT(5); sp = ::new INT(6); then a memory leak will occur.
	template <class D>
	explicit SharedPtr(D *ptr) : m_ptr(ptr), m_counter(ptr)
	{
	}

	template <class D, class Dtr>
	SharedPtr(D *ptr, Dtr dtr) : m_ptr(ptr), m_counter(ptr, dtr)
	{
	}

	SharedPtr(CONST SharedPtr &r) : m_ptr(r.m_ptr), m_counter(r.m_counter)
	{
	}

	template <class D>
	SharedPtr(CONST SharedPtr<D> &r) : m_ptr(r.m_ptr), m_counter(r.m_counter)
	{
	}

	template <class D>
	explicit SharedPtr(CONST WeakPtr<D> &r) : m_ptr(r.m_ptr), m_counter(r.m_counter)
	{
		if (m_counter.IsEmpty())
		{
			ASSERT(FALSE);  // for debug mode
			m_ptr = NULL;
		}
	}

	template <class D>
	explicit SharedPtr(std::auto_ptr<D> &r) : m_ptr(r.get()), m_counter(r)
	{
	}

	SharedPtr &operator=(CONST SharedPtr &rhs)
	{
		this_type(rhs).Swap(*this);
		return *this;
	}

	template <class D>
	SharedPtr &operator=(CONST SharedPtr<D> &rhs)
	{
		this_type(rhs).Swap(*this);
		return *this;
	}

	template <class D>
	SharedPtr &operator=(std::auto_ptr<D> &rhs)
	{
		this_type(rhs).Swap(*this);
		return *this;
	}

	// right value reference, r is a temp object
	SharedPtr(SharedPtr &&r) : m_ptr(r.m_ptr), m_counter()
	{
		m_counter.Swap(r.m_counter);
		r.m_ptr = NULL;
	}

	template <class D>
	SharedPtr(SharedPtr<D> &&r) : m_ptr(r.m_ptr), m_counter()
	{
		m_counter.Swap(r.m_counter);
		r.m_ptr = NULL;
	}

	SharedPtr &operator=(SharedPtr &&rhs)
	{
		this_type(static_cast<SharedPtr &&>(rhs)).Swap(*this);
		return *this;
	}

	template <class D>
	SharedPtr &operator=(SharedPtr<D> &&rhs)
	{
		this_type(static_cast<SharedPtr<D> &&>(rhs)).Swap(*this);
		return *this;
	}

	VOID Reset()
	{
		this_type().Swap(*this);
	}

	template <class D>
	VOID Reset(D *ptr)
	{
		ASSERT(NULL == ptr || m_ptr != ptr);
		this_type(ptr).Swap(*this);
	}

	template <class D, class Dtr>
	VOID Reset(D *ptr, Dtr dtr)
	{
		ASSERT(NULL == ptr || m_ptr != ptr);
		this_type(ptr, dtr).Swap(*this);
	}

	T &operator*() CONST
	{
		ASSERT(NULL != m_ptr);
		return *m_ptr;
	}

	T *operator->() CONST
	{
		ASSERT(NULL != m_ptr);
		return m_ptr;
	}

	operator bool() CONST
	{
		return (NULL != m_ptr && !m_counter.IsEmpty());
	}

	bool operator !() CONST
	{
		return (NULL == m_ptr || m_counter.IsEmpty());
	}

	T *Get() CONST
	{
		return m_ptr;
	}

	BOOL IsUnique() CONST
	{
		return m_counter.IsUnique();
	}

	LONG GetStrongRefCounter() CONST
	{
		return m_counter.GetStrongRefCounter();
	}

	LONG GetWeakRefCounter() CONST
	{
		return m_counter.GetWeakRefCounter();
	}

	VOID Swap(SharedPtr &r)
	{
		g_Swap(m_ptr, r.m_ptr);
		m_counter.Swap(r.m_counter);
	}
};

template <class T>
inline bool operator==(CONST SharedPtr<T> &lhs, CONST SharedPtr<T> &rhs)
{
	return lhs.Get() == rhs.Get();
}

template <class T>
inline bool operator!=(CONST SharedPtr<T> &lhs, CONST SharedPtr<T> &rhs)
{
	return lhs.Get() != rhs.Get();
}

template <class T1, class T2>
inline bool operator==(CONST SharedPtr<T1> &lhs, CONST SharedPtr<T2> &rhs)
{
	return lhs.Get() == rhs.Get();
}

template <class T1, class T2>
inline bool operator!=(CONST SharedPtr<T1> &lhs, CONST SharedPtr<T2> &rhs)
{
	return lhs.Get() != rhs.Get();
}

template <class T>
inline VOID g_SwapSP(CONST SharedPtr<T> &lhs, CONST SharedPtr<T> &rhs)
{
	return lhs.Swap(rhs);
}

template<class T> 
class WeakPtr
{
public:
	typedef T elem_type;
private:
	template <class D> friend class SharedPtr;
	template <class D> friend class WeakPtr;
	typedef WeakPtr<T> this_type;

private:
	T *m_ptr;
	WeakCounter m_counter;

private:
	template<class D> WeakPtr(D *ptr);

public:
	WeakPtr() : m_ptr(NULL), m_counter()
	{
	}

	WeakPtr(CONST WeakPtr &r) : m_ptr(r.Lock().Get()), m_counter(r.m_counter)
	{
	}

	template <class D>
	WeakPtr(CONST WeakPtr<D> &r) : m_ptr(r.Lock().Get()), m_counter(r.m_counter)
	{
	}

	template <class D>
	WeakPtr(CONST SharedPtr<D> &r) : m_ptr(r.m_ptr), m_counter(r.m_counter)
	{
	}

	WeakPtr &operator=(CONST WeakPtr &rhs)
	{
		m_ptr = rhs.Lock().Get();
		m_counter = rhs.m_counter;
		return *this;
	}

	template<class D>
	WeakPtr &operator=(CONST WeakPtr<D> &rhs)
	{
		m_ptr = rhs.Lock().Get();
		m_counter = rhs.m_counter;
		return *this;
	}

	template<class D>
	WeakPtr &operator=(CONST SharedPtr<D> &rhs)
	{
		m_ptr = rhs.m_ptr;
		m_counter = rhs.m_counter;
		return *this;
	}

	// right value reference, r is a temp object
	WeakPtr(WeakPtr &&r) : m_ptr(r.Lock().Get()), m_counter()
	{
		m_counter.Swap(r.m_counter);
		r.m_ptr = NULL;
	}
  
	template <class D>
	WeakPtr(WeakPtr<D> &&r) : m_ptr(r.Lock().Get()), m_counter()
	{
		m_counter.Swap(r.m_counter);
		r.m_ptr = NULL;
	}
	
	WeakPtr &operator=(WeakPtr &&rhs)
	{
		this_type(static_cast<WeakPtr &&>(rhs)).Swap(*this);
		return *this;
	}

	template <class D>
	WeakPtr &operator=(WeakPtr<D> &&rhs)
	{
		this_type(static_cast<WeakPtr<D> &&>(rhs)).Swap(*this);
		return *this;
	}

	SharedPtr<T> Lock() CONST
	{
		return SharedPtr<T>(*this);
	}

	BOOL IsEpired() CONST
	{
		return 0 == m_counter.GetStrongRefCounter();
	}

	LONG GetStrongRefCounter() CONST
	{
		return m_counter.GetStrongRefCounter();
	}

	LONG GetWeakRefCounter() CONST
	{
		return m_counter.GetWeakRefCounter();
	}

	VOID Reset()
	{
		WeakPtr().swap(*this);
	}

	VOID Swap(WeakPtr &r)
	{
		g_Swap(m_ptr, r.m_ptr);
		m_counter.Swap(r.m_counter);
	}
};

template <class T>
inline bool operator==(CONST WeakPtr<T> &lhs, CONST WeakPtr<T> &rhs)
{
	return lhs.Get() == rhs.Get();
}

template <class T>
inline bool operator!=(CONST WeakPtr<T> &lhs, CONST WeakPtr<T> &rhs)
{
	return lhs.Get() != rhs.Get();
}

template <class T1, class T2>
inline bool operator==(CONST WeakPtr<T1> &lhs, CONST WeakPtr<T2> &rhs)
{
	return lhs.Get() == rhs.Get();
}

template <class T1, class T2>
inline bool operator!=(CONST WeakPtr<T1> &lhs, CONST WeakPtr<T2> &rhs)
{
	return lhs.Get() != rhs.Get();
}

template <class T>
inline VOID g_SwapSP(CONST WeakPtr<T> &lhs, CONST WeakPtr<T> &rhs)
{
	return lhs.Swap(rhs);
}

#endif //__COMMON_SHARED_PTR_H__
