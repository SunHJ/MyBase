#include "SocketStreamBuffer.h"


SocketStreamBuffer::SocketStreamBuffer() : m_pBuffer(NULL), m_nTotalSize(DEFAULT_MAX_SOCKET_STREAM_BUFFER), 
	m_nDataHeadPos(0), m_nDataTailPos(0)
{
	while (NULL == m_pBuffer)
	{
		m_pBuffer = (PBYTE)g_AllocMemeoy(m_nTotalSize);
	}
}

SocketStreamBuffer::SocketStreamBuffer(CONST size_t nTotalSize) : m_pBuffer(NULL), m_nTotalSize(nTotalSize),
	m_nDataHeadPos(0), m_nDataTailPos(0)
{
	ASSERT(nTotalSize > 0);
	while (NULL == m_pBuffer)
	{
		m_pBuffer = (PBYTE)g_AllocMemeoy(m_nTotalSize);
	}
}

SocketStreamBuffer::~SocketStreamBuffer()
{
	g_FreeMemeoy(m_pBuffer);
}

size_t SocketStreamBuffer::GetTotalSize() CONST
{
	ASSERT(m_nTotalSize>0);
	return m_nTotalSize;
}

size_t SocketStreamBuffer::GetUsedSize() CONST
{
	ASSERT(m_nTotalSize > 0 && m_nDataTailPos <= m_nTotalSize);
	return m_nDataTailPos;
}

size_t SocketStreamBuffer::GetLeftSize() CONST
{
	ASSERT(m_nTotalSize>0 && m_nDataTailPos<=m_nTotalSize);
	return m_nTotalSize-GetUsedSize();
}

size_t SocketStreamBuffer::GetUnReadDataSize() CONST
{
	ASSERT(m_nTotalSize>0 && m_nDataTailPos<=m_nTotalSize);
	ASSERT(m_nDataHeadPos <= m_nDataTailPos);
	return (m_nDataTailPos - m_nDataHeadPos);
}

PBYTE SocketStreamBuffer::GetLeftPtr() CONST
{
	size_t nUsedSize = GetUsedSize();
	ASSERT(m_nTotalSize>0 && nUsedSize<m_nTotalSize);
	PBYTE pLeft = static_cast<PBYTE>(m_pBuffer);
	pLeft = pLeft + nUsedSize;
	return pLeft;
}

BOOL SocketStreamBuffer::AddUsedSize(CONST size_t nDataBytes)
{
	ASSERT(m_nTotalSize>0 && nDataBytes<=GetLeftSize());
	m_nDataTailPos += nDataBytes;
	return TRUE;
}

// return TRUE:one complete package is received FALSE:no complete package
BOOL SocketStreamBuffer::GetPackage(SPDynamicBuffer &spBuffer)
{
	BOOL   bRetCode = FALSE;
	PBYTE  pPackage = NULL;
	size_t nPackageSize		= 0;
	size_t nPackageHeadSize = sizeof(WORD);
	size_t nPackageBodySize = 0;
	// 检查未读取的数据是否有效
	if (GetUnReadDataSize() >= nPackageHeadSize)
	{
		pPackage = m_pBuffer + m_nDataHeadPos;
		nPackageBodySize = *(WORD *)(pPackage);
		nPackageSize = nPackageBodySize + nPackageHeadSize;
	}
	// 
	if (GetUnReadDataSize() >= nPackageSize)
	{
		PBYTE  pData	 = pPackage + nPackageHeadSize;
		size_t nDataSize = nPackageSize - nPackageHeadSize;
		spBuffer->InsertDataIntoTail(pData, nDataSize);
		m_nDataHeadPos += nPackageSize;
		bRetCode = TRUE;
	}

	if (m_nDataTailPos == m_nTotalSize)
	{
		size_t nUnReadDataSize = m_nDataTailPos - m_nDataHeadPos;
		if (nUnReadDataSize > 0)
			::memmove(m_pBuffer, m_pBuffer+m_nDataHeadPos, nUnReadDataSize);
		m_nDataHeadPos = 0;
		m_nDataTailPos = nUnReadDataSize;
	}

	return bRetCode;
}
