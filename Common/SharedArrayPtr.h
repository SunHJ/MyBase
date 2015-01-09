#ifndef __COMMON_SHARED_ARRAY_PTR_H__
#define __COMMON_SHARED_ARRAY_PTR_H__

#include "RefCounter.h"

template <class T>
class SharedArrayPtr
{
private:
	typedef VOID (*Dtr)(T *&p);
	typedef SharedArrayPtr<T> this_type;
public:
	typedef T elem_type;

private:
	T *m_ptr;
	StrongCounter m_counter;

public:
	// the "explicit" keyword is necessary, or user may mistake to use BSharedArrayPtr like this:
	// BSharedArrayPtr<INT> sp = ::new INT[5]; sp = ::new INT[6]; then a memory leak will occur.
	explicit SharedArrayPtr(T *ptr = NULL) : m_ptr(ptr), m_counter(ptr, g_SafelyDeleteArrayPtr<T>)
	{
	}

	template <class Dtr>
	SharedArrayPtr(T *ptr, Dtr dtr) : m_ptr(ptr), m_counter(ptr, dtr)
	{
	}

	SharedArrayPtr(CONST SharedArrayPtr &r) : m_ptr(r.m_ptr), m_counter(r.m_counter)
	{
	}

	SharedArrayPtr &operator=(CONST SharedArrayPtr &rhs)
	{
		this_type(rhs).Swap(*this);
		return *this;
	}

	VOID Reset(T *ptr = NULL)
	{
		ASSERT(NULL == ptr || m_ptr != ptr);
		this_type(ptr).Swap(*this);
	}

	template <class Dtr>
	VOID Reset(T *ptr, Dtr dtr)
	{
		ASSERT(NULL == ptr || m_ptr != ptr);
		this_type(ptr, dtr).Swap(*this);
	}

	VOID Swap(SharedArrayPtr &r)
	{
		g_Swap(m_ptr, r.m_ptr);
		m_counter.Swap(r.m_counter);
	}

	T &operator[](std::ptrdiff_t i) CONST
	{
		ASSERT(m_ptr != NULL && i >= 0);
		return m_ptr[i];
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
};

template <class T>
inline bool operator==(CONST SharedArrayPtr<T> &lhs, CONST SharedArrayPtr<T> &rhs)
{
	return lhs.Get() == rhs.Get();
}

template <class T>
inline bool operator!=(CONST SharedArrayPtr<T> &lhs, CONST SharedArrayPtr<T> &rhs)
{
	return lhs.Get() != rhs.Get();
}

template <class T>
inline VOID g_SwapSP(CONST SharedArrayPtr<T> &lhs, CONST SharedArrayPtr<T> &rhs)
{
	return lhs.Swap(rhs);
}

#endif //__COMMON_SHARED_ARRAY_PTR_H__
