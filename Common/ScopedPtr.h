#ifndef __COMMON_SCOPED_PTR_H__
#define __COMMON_SCOPED_PTR_H__

#include "Global.h"

template <class T>
class ScopedPtr : private UnCopyable
{
private:
	T *m_ptr;

public:
	ScopedPtr(T *ptr = NULL) : m_ptr(ptr)
	{
	}

	~ScopedPtr() 
	{
		g_SafelyDeletePtr(m_ptr);
	}

public:
	T *GetPtr() CONST
	{
		return m_ptr;
	}

	VOID ResetPtr(T *ptr = NULL)
	{
		if (m_ptr != ptr)
			ScopedPtr<T>(ptr).Swap(*this);
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
		return NULL != m_ptr;
	}

	bool operator !() CONST
	{
		return NULL == m_ptr;
	}

private:
	bool operator==(CONST ScopedPtr &);
	bool operator!=(CONST ScopedPtr &);

	VOID Swap(ScopedPtr &r)
	{
		g_Swap(m_ptr, r.m_ptr);
	}
};

#endif //__COMMON_SCOPED_PTR_H__