#ifndef __COMMON_LINKED_LIST_H__
#define __COMMON_LINKED_LIST_H__

#include "Global.h"

template <class T>
struct LinkedNode
{
	INT m_nPrev;
	INT m_nNext;
	T   m_tValue;

	LinkedNode() : m_nPrev(-1), m_nNext(-1)
	{
	}
};

template <class T>
class FixedLengthLinkedList
{
private:
	typedef LinkedNode<T>* PLinkedNode;

private:
	PLinkedNode m_pLinkedNode;
	INT         m_nElemCount;
	INT         m_nMaxSize;

private:
	FixedLengthLinkedList();
public:
	explicit FixedLengthLinkedList(INT nMaxSize) : m_pLinkedNode(NULL), m_nElemCount(0), m_nMaxSize(nMaxSize)
	{
		ASSERT(m_nMaxSize > 0);
		while (NULL == m_pLinkedNode)
		{
			m_pLinkedNode = ::new LinkedNode<T>[m_nMaxSize];
		}
	}

	~FixedLengthLinkedList()
	{
		g_SafelyDeleteArrayPtr(m_pLinkedNode);
	}

public:
	INT GetElemCount() CONST
	{
		return m_nElemCount;
	}

	INT GetMaxSize() CONST
	{
		return m_nMaxSize;
	}

	PLinkedNode GetNodePtr(CONST INT nIdx) CONST
	{
		ASSERT(NULL != m_pLinkedNode);
		if ( nIdx < 0 || nIdx >= m_nElemCount )
			return NULL;

		return &m_pLinkedNode[nIdx];
	}

	INT GetFreeNodeIdx() CONST
	{
		CHECK_RETURN_CODE((NULL != m_pLinkedNode)    , -1);
		CHECK_RETURN_CODE((m_nElemCount < m_nMaxSize), -1);

		for (INT i = 0; i < m_nMaxSize; i++)
		{
			PLinkedNode pNode = GetNodePtr(i);
			CHECK_RETURN_CODE((NULL != pNode), -1);

			if (pNode->m_nPrev == -1 && pNode->m_nNext == -1)
				return i;
		}

		return -1;
	}

	INT GetNextNodeIdx(CONST INT nIdx) CONST
	{
		CHECK_RETURN_CODE((NULL != m_pLinkedNode)           , -1);
		CHECK_RETURN_CODE((nIdx >= 0 && nIdx < m_nElemCount), -1);
		
		return m_pLinkedNode[nIdx].m_nNext;
	}

	INT GetPrevNodeIdx(CONST INT nIdx) CONST
	{
		CHECK_RETURN_CODE((NULL != m_pLinkedNode)           , -1);
		CHECK_RETURN_CODE((nIdx >= 0 && nIdx < m_nElemCount), -1);
		
		return m_pLinkedNode[nIdx].m_nPrev;
	}

	PLinkedNode GetNextNodePtr(CONST INT nIdx) CONST
	{
		INT nNodeId = GetNextNodeIdx(nIdx);
		return GetNodePtr(nNodeId);
	}

	PLinkedNode GetPrevNodePtr(CONST INT nIdx) CONST
	{
		INT nNodeId = GetPrevNodeIdx(nIdx);
		return GetNodePtr(nNodeId);
	}

	BOOL InsertNode(CONST INT nIdx, CONST T &tValue)
	{
		CHECK_RETURN_BOOL(NULL != m_pLinkedNode);
		CHECK_RETURN_BOOL(m_nElemCount < m_nMaxSize);
		CHECK_RETURN_BOOL(nIdx >= 0 && nIdx < m_nMaxSize);
		CHECK_RETURN_BOOL(m_pLinkedNode[nIdx].m_nNext == -1 && m_pLinkedNode[nIdx].m_nPrev == -1);

		m_pLinkedNode[nIdx].m_tValue = tValue;
		m_pLinkedNode[nIdx].m_nNext = m_pLinkedNode[0].m_nNext;
		m_pLinkedNode[ m_pLinkedNode[0].m_nNext ].m_nPrev = nIdx;
		m_pLinkedNode[0].m_nNext = nIdx;
		m_pLinkedNode[nIdx].m_nPrev = 0;
		m_nElemCount++;
	}

	BOOL RemoveNode(CONST INT nIdx)
	{
		CHECK_RETURN_BOOL(NULL != m_pLinkedNode);
		CHECK_RETURN_BOOL(m_nElemCount <= m_nMaxSize);
		CHECK_RETURN_BOOL(nIdx >= 0 && nIdx < m_nMaxSize);
		CHECK_RETURN_BOOL(m_pLinkedNode[nIdx].m_nNext != -1 && m_pLinkedNode[nIdx].m_nPrev != -1);

		m_pLinkedNode[ m_pLinkedNode[nIdx].m_nPrev ].m_nNext = m_pLinkedNode[nIdx].m_nNext;
		m_pLinkedNode[ m_pLinkedNode[nIdx].m_nNext ].m_nPrev = m_pLinkedNode[nIdx].m_nPrev;
		m_pLinkedNode[nIdx].m_nNext = -1;
		m_pLinkedNode[nIdx].m_nPrev = -1;
		m_nElemCount--;
	}
};

#endif //__COMMON_LINKED_LIST_H__
