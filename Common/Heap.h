#ifndef __COMMOM_HEAP_H__
#define __COMMOM_HEAP_H__

#include "Global.h"

class Heap
{
	enum CompareResult
	{
		LESS = -1,
		EQUAL = 0,
		MORE = 1,
	};
public:
	Heap(INT nSize = DEFAULT_HEAP_SIZE, BOOL bMaxHeap = TRUE);
	~Heap();

	BOOL Heap_Push(INT nValue);
	BOOL Heap_Pop(INT nValue);
	BOOL HasValue(INT nValue);
	INT FindMax();
	INT FindMin();

	VOID PrintHeap();

private:
	inline CompareResult CompareTwo(INT nPos1, INT nPos2)
	{
		return m_pData[nPos1] == m_pData[nPos2] ? EQUAL : (m_pData[nPos1] > m_pData[nPos2] ? MORE : LESS);
	}
	INT FindMinPos(INT nCurPos);
	INT FindMaxPos(INT nCurPos);
	INT FindEqualPos(INT nCurPos);
	INT HeapUp(INT nPos);
	INT HeadDown(INT nPos);	   

private:
	INT m_nSize;
	INT m_nCurPos;
	INT * m_pData;
	CompareResult m_eFlag;
}; 

#endif
