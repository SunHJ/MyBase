#ifndef __COMMON_STRUCTURE_BUFFER_H__
#define __COMMON_STRUCTURE_BUFFER_H__

#include "Type.h"
#include "SharedPtr.h"

class FixedSizeBuffer
{
protected:
	PVOID  m_pvData;            // ptr to the buffer
	size_t m_nTotalSize;        // total size of the buffer
	size_t m_nUsedSize;         // used size of the buffer

public:
	FixedSizeBuffer();
	explicit FixedSizeBuffer(CONST size_t nTotalSize);
	FixedSizeBuffer(CONST PVOID cpvData, CONST size_t nTotalSize);
	virtual ~FixedSizeBuffer();

public:
	PVOID  GetDataPtr(CONST size_t nPos = 0) CONST;
	PVOID  GetLeftPtr() CONST;

	size_t GetTotalSize() CONST;
	size_t GetUsedSize()  CONST;
	size_t GetLeftSize()  CONST;

	BOOL InsertDataIntoTail(CPCVOID cpcData, CONST size_t nDataBytes);
	BOOL InsertDataIntoHead(CPCVOID cpcData, CONST size_t nDataBytes);
	BOOL InsertDataIntoPos (CONST size_t nPos, CPCVOID cpcData, CONST size_t nDataBytes);

	VOID Reset();
};

class DynamicBuffer
{
protected:
	PVOID  m_pvData;            // ptr to the buffer
	size_t m_nTotalSize;        // total size of the buffer
	size_t m_nUsedSize;         // used size of the buffer

public:
	DynamicBuffer();
	explicit DynamicBuffer(CONST size_t nTotalSize);
	DynamicBuffer(CONST PVOID cpvData, CONST size_t nTotalSize);
	virtual ~DynamicBuffer();

public:
	PVOID  GetDataPtr(CONST size_t nPos = 0) CONST;
	PVOID  GetLeftPtr() CONST;

	size_t GetTotalSize() CONST;
	size_t GetUsedSize()  CONST;
	size_t GetLeftSize()  CONST;

	BOOL InsertDataIntoTail(CPCVOID cpcData, CONST size_t nDataBytes);
	BOOL InsertDataIntoHead(CPCVOID cpcData, CONST size_t nDataBytes);
	BOOL InsertDataIntoPos (CONST size_t nPos, CPCVOID cpcData, CONST size_t nDataBytes);

	VOID Reset();

protected:
	BOOL ReAllocate();
};

typedef       DynamicBuffer *       PDynamicBuffer;
typedef CONST DynamicBuffer *       PCDynamicBuffer;
typedef       DynamicBuffer * CONST CPDynamicBuffer;
typedef CONST DynamicBuffer * CONST CPCDynamicBuffer;
typedef SharedPtr<DynamicBuffer> SPDynamicBuffer;

inline VOID g_CopyBufferData(DynamicBuffer &dst, DynamicBuffer &src)
{
	dst.Reset();
	dst.InsertDataIntoTail(src.GetDataPtr(), src.GetUsedSize());
}

inline VOID g_CopyBufferData(DynamicBuffer &dst, DynamicBuffer &src, CONST size_t nDataBytes)
{
	dst.Reset();
	dst.InsertDataIntoTail(src.GetDataPtr(), nDataBytes);
}

inline VOID g_CopyBufferData(SPDynamicBuffer &spDst, SPDynamicBuffer &spSrc)
{
	spDst->Reset();
	spDst->InsertDataIntoTail(spSrc->GetDataPtr(), spSrc->GetUsedSize());
}

inline VOID g_CopyBufferData(SPDynamicBuffer &spDst, SPDynamicBuffer &spSrc, CONST size_t nDataBytes)
{
	spDst->Reset();
	spDst->InsertDataIntoTail(spSrc->GetDataPtr(), nDataBytes);
}

#endif //__COMMON_STRUCTURE_BUFFER_H__
