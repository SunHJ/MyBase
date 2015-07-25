#ifndef __BASE_ENGINE_KG_SMARTPOINTER_H__
#define __BASE_ENGINE_KG_SMARTPOINTER_H__

#include "RefCounter.h"

template <class T>
class SmartPointer
{
private:
    typedef T               elem_type;
    typedef SmartPointer<T> this_type;
    template <class D> friend class SmartPointer;

private:
    T *m_pPointer;

public:
    // 构造析构函数
    SmartPointer(T *p = NULL);                                    // 默认构造函数 or 带1个参数的构造函数
    SmartPointer(const SmartPointer<T> &r);                       // 拷贝构造函数
    ~SmartPointer();                                              // 析构函数

    template <class D>
    SmartPointer(const SmartPointer<D> &r) : m_pPointer(r.m_pPointer)
    {
        if (0 != m_pPointer)
        {
            m_pPointer->AddRef();
        }
    }

public:
    // 运算符重载
    SmartPointer<T> &		operator=(const SmartPointer<T> &rhs);		// operator=
    T &						operator*()  const;                         // operator*
    T *						operator->() const;                         // operator->
    bool					operator !() const;                         // operator!
    operator bool() const;                                              // 对象到bool的隐式转换

    template <class D>
    SmartPointer<T> & operator=(const SmartPointer<D> &rhs)
    {  // 如果rhs就是自身对象，那么先增加引用计数，然后再减少引用计数，不能反过来，否则对象会析构。
        if (0 != rhs.m_pPointer)
        {
            rhs.m_pPointer->AddRef();
        }

        if (0 != m_pPointer)
        {
            m_pPointer->DecRef();
        }

        m_pPointer = rhs.m_pPointer;
        return *this;
    }

    // 成员函数
    T *  Get() const;                                                   // 获取m_pPointer
    void Reset(T *p);                                                   // 重置

    // 友元函数 : 必须定义在模板类内部
    friend bool operator==(const SmartPointer<T> &lhs, const SmartPointer<T> &rhs)
    {
        return (lhs.m_pPointer == rhs.m_pPointer);
    }

    friend bool operator!=(const SmartPointer<T> &lhs, const SmartPointer<T> &rhs)
    {
        return (lhs.m_pPointer != rhs.m_pPointer);
    }

    friend bool operator<(const SmartPointer<T> &lhs, const SmartPointer<T> &rhs)
    {
        return (lhs.m_pPointer < rhs.m_pPointer);
    }

    friend bool operator>(const SmartPointer<T> &lhs, const SmartPointer<T> &rhs)
    {
        return (lhs.m_pPointer > rhs.m_pPointer);
    }

    friend bool operator<=(const SmartPointer<T> &lhs, const SmartPointer<T> &rhs)
    {
        return (lhs.m_pPointer <= rhs.m_pPointer);
    }

    friend bool operator>=(const SmartPointer<T> &lhs, const SmartPointer<T> &rhs)
    {
        return (lhs.m_pPointer >= rhs.m_pPointer);
    }
};

template <class T>
inline SmartPointer<T>::SmartPointer(T *p) : m_pPointer(p)
{
    if (0 != m_pPointer)
    {
        m_pPointer->AddRef();
    }
}

template <class T>
inline SmartPointer<T>::SmartPointer(const SmartPointer<T> &r) : m_pPointer(r.m_pPointer)
{
    if (0 != m_pPointer)
    {
        m_pPointer->AddRef();
    }
}

template <class T>
inline SmartPointer<T>::~SmartPointer()
{
    if (0 != m_pPointer)
    {
        m_pPointer->DecRef();
    }
}

template <class T>
inline SmartPointer<T> & SmartPointer<T>::operator=(const SmartPointer<T> &rhs)
{ // 如果是自身对象赋值，那么不进行任何操作。
    if (this != &rhs)
    { // not self
        if (0 != rhs.m_pPointer)
        {
            rhs.m_pPointer->AddRef();
        }

        if (0 != m_pPointer)
        {
            m_pPointer->DecRef();
        }

        m_pPointer = rhs.m_pPointer;
    }

    return *this;
}

template <class T>
inline T & SmartPointer<T>::operator*() const
{
    if (0 == m_pPointer)
    {
        KG_ASSERT(false && "[ERROR] KG_SmartPointer can't use \"*\" operator when m_pPointer is null.");
    }

    return *m_pPointer;
}

template <class T>
inline T * SmartPointer<T>::operator->() const
{
    if (0 == m_pPointer)
    {
        ASSERT(false && "[WARNING] KG_SmartPointer may raise some exceptions, using \"->\" operator when m_pPointer is null.");
    }

    return m_pPointer;
}

template <class T>
inline bool SmartPointer<T>::operator !() const
{
    return (0 == m_pPointer);
}

template <class T>
inline SmartPointer<T>::operator bool() const
{
    return (0 != m_pPointer);
}

template <class T>
inline T * SmartPointer<T>::Get() const
{
    return m_pPointer;
}

template <class T>
inline void SmartPointer<T>::Reset(T *p)
{ // 如果p就是自身对象Get()出来，那么先增加引用计数，然后再减少引用计数，不能反过来，否则对象会析构。
    if (0 != p)
    {
        p->AddRef();
    }

    if (0 != m_pPointer)
    {
        m_pPointer->DecRef();
        m_pPointer = p;
    }
}
#endif  // __BASE_ENGINE_KG_SMARTPOINTER_H__
