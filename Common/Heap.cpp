#include "Heap.h"

Heap::Heap(INT nSize /*= DEFAULT_HEAP_SIZE*/, BOOL bMaxHeap /*= TRUE*/)
{
	m_nSize = nSize;
	m_nCurPos = 0;
	m_pData = new INT[nSize+1];
	m_eFlag = bMaxHeap ? LESS : MORE;
}

Heap::~Heap()
{
	g_SafelyDeleteArrayPtr(m_pData);
}

BOOL Heap::Heap_Push(INT nValue)
{
	INT nPos = m_nCurPos + 1;
	CHECK_RETURN_BOOL(nPos <= m_nSize);

	m_pData[nPos] = nValue;
	HeapUp(nPos);

	m_nCurPos += 1;
	return TRUE;
}

BOOL Heap::Heap_Pop(INT nValue)
{
	INT nPos, nRetCode;
	m_pData[0] = nValue;
	nPos = FindEqualPos(1);
	CHECK_RETURN_BOOL_QUIET(nPos > 0);

	g_Swap(m_pData[nPos], m_pData[m_nCurPos]);

	nRetCode = HeapUp(nPos);
	if (0 == nRetCode)
	{
		nRetCode = HeadDown(nPos);
	}

	m_nCurPos -= 1;
	return TRUE;
}

BOOL Heap::HasValue(INT nValue)
{
	INT nPos = 0;

	m_pData[0] = nValue;	
	nPos = FindEqualPos(1);

	return (nPos > 0);
}

INT Heap::FindMax()
{
	INT nPos = FindMaxPos(1);
	if (0 == nPos)
	{
		return INT_MAX;
	}
	return m_pData[nPos];
}
INT Heap::FindMin()
{
	INT nPos = FindMinPos(1);
	if (0 == nPos)
	{
		return INT_MIN;
	}
	return m_pData[nPos];
}


INT Heap::FindEqualPos(INT nCurPos)
{
	INT nRePos = 0;
	if (nCurPos > m_nCurPos)
	{
		return nRePos;
	}
	CompareResult eResult = CompareTwo(nCurPos, 0);
	if (EQUAL == eResult)
	{
		return nCurPos;
	}
	else if (m_eFlag == eResult)
	{
		return nRePos;
	}
	else
	{
		nRePos = FindEqualPos(nCurPos * 2);
		if (0 == nRePos)
		{
			nRePos = FindEqualPos(nCurPos * 2 + 1);
		}
	}
	return nRePos;
}

INT Heap::HeapUp(INT nPos)
{
	if (nPos > 1)
	{
		INT nUpPos = nPos / 2;
		if ( m_eFlag == CompareTwo(nUpPos, nPos)) // m_pData[nUpPos] < m_pData[nPos])
		{
			g_Swap(m_pData[nUpPos], m_pData[nPos]);
			return HeapUp(nUpPos) + 1;
		}
	}
	return 0;
}

INT Heap::HeadDown(INT nPos)
{
	if (nPos < m_nCurPos)
	{
		INT nDownPos = 0, nLPos, nRPos;
		nLPos = nPos * 2;
		if (nLPos > m_nCurPos)
		{
			return 0;
		}
		nRPos = nPos * 2 + 1;
		if (nRPos > m_nCurPos)
		{
			nDownPos = nLPos;
		}
		else
		{
			nDownPos = (m_eFlag == CompareTwo(nLPos, nRPos) ? nRPos : nLPos);
		}
		if (m_eFlag == CompareTwo(nPos, nDownPos))
		{
			g_Swap(m_pData[nPos], m_pData[nDownPos]);
			return HeadDown(nDownPos) + 1;
		}
	}
	return 0;
}

VOID Heap::PrintHeap()
{
	printf("Heap Size : %d\n", m_nSize);
	printf("ItemCount : %d\n", m_nCurPos);
	if (m_nCurPos > 0)
	{
		printf("All Data：\n");
		for (int i = 1; i <= m_nCurPos; i++)
		{
			printf("%2d %d\n", i, m_pData[i]);
		}
		printf("\n");
	}
	else
	{
		printf("No Data！\n");
	}
}

INT Heap::FindMinPos(INT nCurPos)
{
	// Heap is Empty
	if (m_nCurPos < 1)
		return 0;

	// MinHeap, return the first index
	if (MORE == m_eFlag)
		return 1;

	INT nLPos, nRPos;
	nLPos = nCurPos * 2;
	// 
	if (nLPos > m_nCurPos)
	{ // 是叶子节点，返回当前index
		return nCurPos;
	}
	nRPos = nCurPos * 2 + 1;
	if (nRPos > m_nCurPos)
	{	// 右子节点为空，返回左子节点最小index = FindMinPos(nLPos))  
		// 因为是完全二叉树，左子节点必为叶子节点，可以返回当前节点的左子节点的index
		return nLPos;
	}

	nLPos = FindMinPos(nLPos);
	nRPos = FindMinPos(nRPos);
	return (m_eFlag == CompareTwo(nLPos, nRPos) ? nLPos : nRPos);
}

INT Heap::FindMaxPos(INT nCurPos)
{
	// Heap is Empty
	if (m_nCurPos < 1)
		return 0;

	// MinHeap, return the first index
	if (LESS == m_eFlag)
		return 1;

	INT nLPos, nRPos;
	nLPos = nCurPos * 2;
	// 
	if (nLPos > m_nCurPos)
	{ // 是叶子节点，返回当前index
		return nCurPos;
	}
	nRPos = nCurPos * 2 + 1;
	if (nRPos > m_nCurPos)
	{	// 右子节点为空，返回左子节点最小index = FindMinPos(nLPos))  
		// 因为是完全二叉树，左子节点必为叶子节点，可以返回当前节点的左子节点的index
		return nLPos;
	}

	nLPos = FindMaxPos(nLPos);
	nRPos = FindMaxPos(nRPos);
	return (m_eFlag == CompareTwo(nLPos, nRPos) ? nLPos : nRPos);
}

