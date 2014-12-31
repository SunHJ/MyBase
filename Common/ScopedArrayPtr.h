#ifndef __COMMON_SCOPED_ARRAY_PTR_H__
#define __COMMON_SCOPED_ARRAY_PTR_H__

#include "Global.h"

template <class T>
class ScopedArrayPtr : private UnCopyable
{
private:
	T *m_ptr;

public:
	ScopedArrayPtr(T *ptr = NULL) : m_ptr(ptr)
	{
	}

	~ScopedArrayPtr()
	{
		g_SafelyDeleteArrayPtr(m_ptr);
	}

public:
	T *GetPtr() CONST
	{
		return m_ptr;
	}

	VOID ResetPtr(T *ptr = NULL)
	{
		if (m_ptr != ptr)
			ScopedArrayPtr<T>(ptr).Swap(*this);
	}

	T &operator[](CONST UINT pos) CONST
	{
		ASSERT(NULL != m_ptr && pos >= 0);
		return m_ptr[pos];
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
	bool operator== (CONST ScopedArrayPtr &);
	bool operator!= (CONST ScopedArrayPtr &);

	VOID Swap(ScopedArrayPtr &r)
	{
		g_Swap(m_ptr, r.m_ptr);
	}
};

#endif //__COMMON_SCOPED_ARRAY_PTR_H__