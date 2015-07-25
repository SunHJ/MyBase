#ifndef __PUBLIC_STRUCT_H__
#define __PUBLIC_STRUCT_H__

#include <queue>
#include <vector>

#include "CriticalSection.h"


template <class T>
void gSafeDeletePtr(T* pT)
{
	if (NULL != pT)
	{
		delete pT;
		pT = NULL;
	}
}

template <class T>
class MTQueue
{
private:
	std::queue<T*> This;
    CriticalSection m_csV;

public:
	MTQueue() {
	};
	~MTQueue() {
	};

	void Push(T* &pItem) {
        MutexGuard tmpGuard(m_csV);
		This.push(pItem);
	}

	T* Pop() {
		static T* pItem = NULL;
        MutexGuard tmpGuard(m_csV);
		if (!This.empty())
		{
			pItem = This.front();
			This.pop();
		}
		return pItem;
	}
	
	int Size() const {
		return (int)This.size();
	}

	bool Empty() { 
		return This.empty();
	}
};

template <class T>
class MTVector
{
	typedef std::vector<T*> TemplateVector;
	typedef void(*DestoryFun)(T*);
private:
    TemplateVector This;
    CriticalSection m_csV;

public:
	MTVector() {
	};
	~MTVector() {
	};

    void Push(T* &pItem) {
        MutexGuard tmpGuard(m_csV);
		This.push_back(pItem);
	}

	bool Earse(T* pItem) {
        bool bReslut = false;
        MutexGuard tmpGuard(m_csV);
		TemplateVector::iterator itResult = find(This.begin(), This.end(), pItem);
		if (itResult != This.end())
		{
			bReslut = true;
			This.erase(itResult);
		}
		return bReslut;
	}

	void Clear(DestoryFun pFun = NULL)
    {
        MutexGuard tmpGuard(m_csV);
		if (NULL == pFun)
		{
			This.clear();
		}
		else
		{
			for (TemplateVector::iterator it = This.begin(); it != This.end(); it++)
			{
				pFun(*it);
			}
		}
	}

	int Size() const {
		return (int)This.size();
	}

	bool Empty() const {
		return This.empty();
	}
};

#endif