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
    // ������������
    SmartPointer(T *p = NULL);                                    // Ĭ�Ϲ��캯�� or ��1�������Ĺ��캯��
    SmartPointer(const SmartPointer<T> &r);                       // �������캯��
    ~SmartPointer();                                              // ��������

    template <class D>
    SmartPointer(const SmartPointer<D> &r) : m_pPointer(r.m_pPointer)
    {
        if (0 != m_pPointer)
        {
            m_pPointer->AddRef();
        }
    }

public:
    // ���������
    SmartPointer<T> &		operator=(const SmartPointer<T> &rhs);		// operator=
    T &						operator*()  const;                         // operator*
    T *						operator->() const;                         // operator->
    bool					operator !() const;                         // operator!
    operator bool() const;                                              // ����bool����ʽת��

    template <class D>
    SmartPointer<T> & operator=(const SmartPointer<D> &rhs)
    {  // ���rhs�������������ô���������ü�����Ȼ���ټ������ü��������ܷ���������������������
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

    // ��Ա����
    T *  Get() const;                                                   // ��ȡm_pPointer
    void Reset(T *p);                                                   // ����

    // ��Ԫ���� : ���붨����ģ�����ڲ�
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
{ // ������������ֵ����ô�������κβ�����
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
{ // ���p�����������Get()��������ô���������ü�����Ȼ���ټ������ü��������ܷ���������������������
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
