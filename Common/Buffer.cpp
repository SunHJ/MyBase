
#include "Buffer.h"

FixedSizeBuffer::FixedSizeBuffer() : 
	m_pvData(NULL), m_nTotalSize(DEFAULT_MAX_BUFFER_LEN), m_nUsedSize(0)
{
	while (NULL == m_pvData)
	{
		m_pvData = ::malloc(m_nTotalSize);
	}
}

FixedSizeBuffer::FixedSizeBuffer(CONST size_t nTotalSize) :	
	m_pvData(NULL),	m_nUsedSize(0)
{
	m_nTotalSize = nTotalSize>0 ? nTotalSize : DEFAULT_MAX_BUFFER_LEN;
	while (NULL == m_pvData)
	{
		m_pvData = ::malloc(m_nTotalSize);
	}
}

FixedSizeBuffer::FixedSizeBuffer(CONST PVOID cpvData, CONST size_t nTotalSize) : 
	m_pvData(cpvData), m_nTotalSize(nTotalSize), m_nUsedSize(0)
{
	if (NULL == m_pvData || m_nTotalSize <= 0)
	{
		g_FreeMemeoy(m_pvData);
		m_nTotalSize = DEFAULT_MAX_BUFFER_LEN;
		while (NULL == m_pvData)
			m_pvData = ::malloc(m_nTotalSize);
	}
}

FixedSizeBuffer::~FixedSizeBuffer()
{
	g_FreeMemeoy(m_pvData);
	m_nTotalSize = 0;
	m_nUsedSize = 0;
}

PVOID FixedSizeBuffer::GetDataPtr(CONST size_t nPos /* = 0 */) CONST
{
	CHECK_RETURN_CODE_QUIET(nPos>=0 && nPos<m_nUsedSize, NULL);
	PCHAR pPos = static_cast<PCHAR>(m_pvData);
	pPos	   = pPos + nPos;
	return static_cast<PVOID>(pPos);
}

PVOID FixedSizeBuffer::GetLeftPtr() CONST
{
	CHECK_RETURN_CODE_QUIET(m_nTotalSize>0 && m_nUsedSize<m_nTotalSize, NULL);
	PCHAR pLeft = static_cast<PCHAR>(m_pvData);
	pLeft		= pLeft + m_nUsedSize;
	return static_cast<PVOID>(pLeft);
}

size_t FixedSizeBuffer::GetTotalSize() CONST
{
	ASSERT(m_nTotalSize > 0);
	return m_nTotalSize;
}

size_t FixedSizeBuffer::GetUsedSize() CONST
{
	ASSERT(m_nTotalSize>0 && m_nUsedSize<=m_nTotalSize);
	return m_nUsedSize;
}

size_t FixedSizeBuffer::GetLeftSize() CONST
{
	ASSERT(m_nTotalSize>0 && m_nUsedSize<=m_nTotalSize);
	return (m_nTotalSize-m_nUsedSize);
}

BOOL FixedSizeBuffer::InsertDataIntoTail(CPCVOID cpcData, CONST size_t nDataBytes)
{
	CHECK_RETURN_BOOL_QUIET(NULL != cpcData && nDataBytes >= 0);
	CHECK_RETURN_BOOL_QUIET(m_nTotalSize>0 && m_nUsedSize<=m_nTotalSize);
	CHECK_RETURN_BOOL_QUIET(m_nUsedSize+nDataBytes <= m_nTotalSize);
	PCHAR pLeft = static_cast<PCHAR>(m_pvData) + m_nUsedSize;
	::memcpy(pLeft, cpcData, nDataBytes);
	
	m_nUsedSize += nDataBytes;
	return TRUE;
}

BOOL FixedSizeBuffer::InsertDataIntoHead(CPCVOID cpcData, CONST size_t nDataBytes)
{
	CHECK_RETURN_BOOL_QUIET(NULL != cpcData && nDataBytes >= 0);
	CHECK_RETURN_BOOL_QUIET(m_nTotalSize>0 && m_nUsedSize<=m_nTotalSize);
	CHECK_RETURN_BOOL_QUIET(m_nUsedSize+nDataBytes <= m_nTotalSize);
	
	CONST PCHAR pData = static_cast<CONST PCHAR>(m_pvData);
	::memmove(pData + nDataBytes, pData, m_nUsedSize);
	::memcpy(pData, cpcData, nDataBytes);

	m_nUsedSize += nDataBytes;
	return TRUE;
}

BOOL FixedSizeBuffer::InsertDataIntoPos(CONST size_t nPos, CPCVOID cpcData, CONST size_t nDataBytes)
{
	CHECK_RETURN_BOOL_QUIET(NULL != cpcData && nDataBytes >= 0);
	CHECK_RETURN_BOOL_QUIET(nPos < m_nUsedSize);
	CHECK_RETURN_BOOL_QUIET(m_nTotalSize>0 && m_nUsedSize<=m_nTotalSize);
	CHECK_RETURN_BOOL_QUIET(m_nUsedSize+nDataBytes <= m_nTotalSize);

	PCHAR pPos = static_cast<PCHAR>(m_pvData) + nPos;
	::memmove(pPos + nDataBytes, pPos, m_nUsedSize);
	::memcpy(pPos, cpcData, nDataBytes);

	m_nUsedSize += nDataBytes;
	return TRUE;
}

VOID FixedSizeBuffer::Reset()
{
	CHECK_RETURN_VOID_QUIET(NULL!=m_pvData && m_nTotalSize>0);
	::memset(m_pvData, 0, m_nTotalSize);
	m_nUsedSize = 0;
}


DynamicBuffer::DynamicBuffer() : 
	m_pvData(NULL), m_nTotalSize(DEFAULT_MAX_BUFFER_LEN), m_nUsedSize(0)
{
	while (NULL == m_pvData)
	{
		m_pvData = ::malloc(m_nTotalSize);
	}
}

DynamicBuffer::DynamicBuffer(CONST size_t nTotalSize) :
	m_pvData(NULL),	m_nUsedSize(0)
{
	m_nTotalSize = nTotalSize > 0 ? nTotalSize : DEFAULT_MAX_BUFFER_LEN;
	while (NULL == m_pvData)
	{
		m_pvData = ::malloc(m_nTotalSize);
	}
}

DynamicBuffer::DynamicBuffer(CONST PVOID cpvData, CONST size_t nTotalSize) :
	m_pvData(cpvData), m_nTotalSize(nTotalSize), m_nUsedSize(0)
{
	if (NULL == m_pvData || m_nTotalSize <=0)
	{
		g_FreeMemeoy(m_pvData);
		m_nTotalSize = DEFAULT_MAX_BUFFER_LEN;
		while (NULL == m_pvData)
		{
			m_pvData = ::malloc(m_nTotalSize);
		}
	}
}

DynamicBuffer::~DynamicBuffer()
{
	g_FreeMemeoy(m_pvData);
	m_nTotalSize = 0;
	m_nUsedSize  = 0;
}

PVOID DynamicBuffer::GetDataPtr(CONST size_t nPos) CONST
{
	CHECK_RETURN_CODE_QUIET(nPos >= 0 && nPos < m_nUsedSize, NULL);
	PCHAR pPos = static_cast<PCHAR>(m_pvData) + nPos;

	return static_cast<PVOID>(pPos);
}

PVOID DynamicBuffer::GetLeftPtr() CONST
{
	CHECK_RETURN_CODE_QUIET(m_nTotalSize > 0 && m_nUsedSize < m_nTotalSize, NULL);
	PCHAR pLeft = static_cast<PCHAR>(m_pvData) + m_nUsedSize;

	return static_cast<PVOID>(pLeft);
}

size_t DynamicBuffer::GetTotalSize() CONST
{
	ASSERT(m_nTotalSize > 0);
	return m_nTotalSize;
}

size_t DynamicBuffer::GetUsedSize() CONST
{
	ASSERT(m_nTotalSize > 0 && m_nUsedSize <= m_nTotalSize);
	return m_nUsedSize;
}

size_t DynamicBuffer::GetLeftSize() CONST
{
	ASSERT(m_nTotalSize > 0 && m_nUsedSize <= m_nTotalSize);
	return m_nTotalSize - m_nUsedSize;
}

VOID DynamicBuffer::Reset()
{
	CHECK_RETURN_VOID_QUIET(NULL != m_pvData && m_nTotalSize > 0);
	::memset(m_pvData, 0, m_nTotalSize);
	m_nUsedSize = 0;
}

VOID DynamicBuffer::SetUseSize(CONST size_t nDataBytes)
{
	ASSERT(nDataBytes < m_nTotalSize);
	m_nUsedSize = nDataBytes;
}

BOOL DynamicBuffer::InsertDataIntoTail(CPCVOID cpcData, CONST size_t nDataBytes)
{
	CHECK_RETURN_BOOL_QUIET(NULL != cpcData && m_nTotalSize > 0 && m_nUsedSize <= m_nTotalSize);
	while (m_nTotalSize - m_nUsedSize < nDataBytes)
	{
		ReAllocate();
	}

	size_t nUsedSize = m_nUsedSize + nDataBytes;
	while (m_nTotalSize - nUsedSize < DEFAULT_MAX_BUFFER_LEN)
	{
		ReAllocate();
	}

	PCHAR pLeft = static_cast<PCHAR>(m_pvData) + m_nUsedSize;
	::memcpy(pLeft, cpcData, nDataBytes);
	m_nUsedSize = nUsedSize;

	return TRUE;
}

BOOL DynamicBuffer::InsertDataIntoHead(CPCVOID cpcData, CONST size_t nDataBytes)
{
	CHECK_RETURN_BOOL_QUIET(NULL != cpcData && m_nTotalSize > 0 && m_nUsedSize <= m_nTotalSize);
	while (m_nTotalSize - m_nUsedSize < nDataBytes)
	{
		ReAllocate();
	}

	CONST PCHAR pData = static_cast<CONST PCHAR>(m_pvData);
	::memmove(pData + nDataBytes, pData, m_nUsedSize);
	::memcpy(pData, cpcData, nDataBytes);

	m_nUsedSize += nDataBytes;
	return TRUE;
}

BOOL DynamicBuffer::InsertDataIntoPos (CONST size_t nPos, CPCVOID cpcData, CONST size_t nDataBytes)
{
	CHECK_RETURN_BOOL_QUIET(nPos >= 0 && nPos < m_nUsedSize);
	CHECK_RETURN_BOOL_QUIET(NULL != cpcData && m_nTotalSize > 0);
	CHECK_RETURN_BOOL_QUIET(m_nUsedSize <= m_nTotalSize);

	while (m_nTotalSize - m_nUsedSize < nDataBytes)
	{
		ReAllocate();
	}

	PCHAR pPos = static_cast<PCHAR>(m_pvData) + nPos;

	::memmove(pPos + nDataBytes, pPos, m_nUsedSize);
	::memcpy(pPos, cpcData, nDataBytes);

	m_nUsedSize += nDataBytes;
	return TRUE;
}

BOOL DynamicBuffer::ReAllocate()
{   // realloc the memory, so m_nTotalSize is changed.
	while (NULL == m_pvData)
	{
		m_pvData = ::malloc(DEFAULT_MAX_BUFFER_LEN);
	}

	PVOID pTemp = m_pvData;
	m_nTotalSize = m_nTotalSize + DEFAULT_MAX_BUFFER_LEN;
	m_pvData = ::realloc(m_pvData, m_nTotalSize);

	if (NULL == m_pvData)
	{
		m_pvData = pTemp;
	}
	return TRUE;
}
