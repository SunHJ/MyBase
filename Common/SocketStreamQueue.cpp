#include "SocketStreamQueue.h"


SocketStreamQueue::SocketStreamQueue() : m_nLastWaitToProcessPos(0)
{
	m_vecSocketStream.clear();
}

SocketStreamQueue::~SocketStreamQueue()
{
	CloseAll();
}

BOOL SocketStreamQueue::SelectWait(INT nMaxEventCount, INT &nEventCount, SPSocketEventArray spEventArray)
{
	CHECK_RETURN_BOOL(nMaxEventCount > 0 && spEventArray && nEventCount <= nMaxEventCount);

	INT nReCode = 0;
	timeval tmTimeOut;
	tmTimeOut.tv_sec = 0;
	tmTimeOut.tv_usec = 0;
	size_t nStreamVectorLen = m_vecSocketStream.size();
	if (m_nLastWaitToProcessPos >= nStreamVectorLen)
		m_nLastWaitToProcessPos = 0;

	SPSocketStreamVector::iterator iterBegin = m_vecSocketStream.begin();
	SPSocketStreamVector::iterator iterEnd = m_vecSocketStream.end();
	SPSocketStreamVector::iterator iterNext = iterBegin;
	std::advance(iterNext, m_nLastWaitToProcessPos);

	size_t nToProcessEventCount = nStreamVectorLen;
	while (nToProcessEventCount--)
	{
		if (iterNext == iterEnd)
			iterNext = iterBegin;

		SPSocketStreamVector::iterator iterTemp = iterNext++;

		SPSocketStream spAsyncSocketStream = *iterTemp;
		nReCode = g_SelectDataIn(spAsyncSocketStream->GetSocket(), &tmTimeOut);
		if (1 == nReCode)
		{
			spEventArray[nEventCount].m_nEventType = SOCKET_EVENT_IN;
			spEventArray[nEventCount].m_spAsyncSocketStream = spAsyncSocketStream;
			nEventCount++;
		}
		else if (-1 == nReCode)
		{
			nReCode = g_GetSocketLastError();
			if (SOCKET_ERROR == nReCode)
			{
				spEventArray[nEventCount].m_nEventType = SOCKET_EVENT_CLOSE;
				spEventArray[nEventCount].m_spAsyncSocketStream = spAsyncSocketStream;
				nEventCount++;
			}
		}
		// 一次循环要处理的请求多余最大并发数 结束
		if (nEventCount >= nMaxEventCount)
		{
			break;
		}
	}
	// 保存当前等待处理节点位置，下次优先处理
	m_nLastWaitToProcessPos = std::distance(iterBegin, iterNext);
	return TRUE;
}

BOOL SocketStreamQueue::PushBack(SPSocketStream &spAsyncSocketStream)
{
	m_vecSocketStream.push_back(spAsyncSocketStream);
	return TRUE;
}

BOOL SocketStreamQueue::Remove(SPSocketStream &spAsyncSocketStream)
{
	SPSocketStreamVector::iterator iterBegin = m_vecSocketStream.begin();
	SPSocketStreamVector::iterator iterEnd = m_vecSocketStream.end();
	SPSocketStreamVector::iterator iter = std::find(iterBegin, iterEnd, spAsyncSocketStream);

	if (iter != iterEnd)
		m_vecSocketStream.erase(iter);

	return TRUE;
}

VOID SocketStreamQueue::CloseAll()
{
	SPSocketStreamVector::iterator iterBegin = m_vecSocketStream.begin();
	while (iterBegin != m_vecSocketStream.end())
	{
		(*iterBegin)->UnInit();
		iterBegin++;
	}
	m_vecSocketStream.clear();
}

size_t SocketStreamQueue::GetCurStreamVectorLen() CONST
{
	return m_vecSocketStream.size();
}

AsyncSocketStreamQueue::AsyncSocketStreamQueue() : m_nLastWaitToProcessPos(0)
{
	m_vecSocketStream.clear();
}

AsyncSocketStreamQueue::~AsyncSocketStreamQueue()
{  
	CloseAll();
}

BOOL AsyncSocketStreamQueue::AddClient(PAsyncSocketStream &pAsyncSocketStream)
{
	m_vecSocketStream.push_back(pAsyncSocketStream);
	return TRUE;
}

BOOL AsyncSocketStreamQueue::DelClient(PAsyncSocketStream &pAsyncSocketStream)
{
	VecPAsyncSocketStream::iterator iterBegin = m_vecSocketStream.begin();
	VecPAsyncSocketStream::iterator iterEnd = m_vecSocketStream.end();
	VecPAsyncSocketStream::iterator iter = std::find(iterBegin, iterEnd, pAsyncSocketStream);

	if (iter != iterEnd)
	{
		m_vecSocketStream.erase(iter);
		g_SafelyDeletePtr(pAsyncSocketStream);
	}

	return TRUE;
}

VOID AsyncSocketStreamQueue::CloseAll()
{
	VecPAsyncSocketStream::iterator iterBegin = m_vecSocketStream.begin();
	VecPAsyncSocketStream::iterator iterEnd = m_vecSocketStream.end();

	PAsyncSocketStream pSocketStream = NULL;
	while (iterBegin != iterEnd)
	{
		pSocketStream = (*iterBegin);
		if (NULL == pSocketStream)
			continue;

		pSocketStream->Close();
		g_SafelyDeletePtr(pSocketStream);

		iterBegin++;
	}
	m_vecSocketStream.clear();
}

size_t AsyncSocketStreamQueue::GetCurStreamVectorLen() CONST
{
	return m_vecSocketStream.size();
}

BOOL AsyncSocketStreamQueue::Wait(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray)
{
	CHECK_RETURN_BOOL(nMaxEventCount > 0 && spEventArray && nEventCount <= nMaxEventCount);

	INT nReCode = 0;
	timeval tmTimeOut;
	tmTimeOut.tv_sec = 0;
	tmTimeOut.tv_usec = 0;
	size_t nStreamVectorLen = m_vecSocketStream.size();
	if (m_nLastWaitToProcessPos >= nStreamVectorLen)
		m_nLastWaitToProcessPos = 0;

	VecPAsyncSocketStream::iterator iterBegin = m_vecSocketStream.begin();
	VecPAsyncSocketStream::iterator iterEnd = m_vecSocketStream.end();
	VecPAsyncSocketStream::iterator iterNext = iterBegin;
	std::advance(iterNext, m_nLastWaitToProcessPos);
	size_t nToProcessEventCount = nStreamVectorLen;
	SocketEventType evType = SOCKET_EVENT_INVALID; 
	while (nToProcessEventCount--)
	{
		if (nEventCount >= nMaxEventCount)
		{
			break;
		}

		if (iterNext == iterEnd)
			iterNext = iterBegin;

		VecPAsyncSocketStream::iterator iterTemp = iterNext++;
		PAsyncSocketStream pAsyncSocketStream = (*iterTemp);
		ASSERT(pAsyncSocketStream);

		evType = SOCKET_EVENT_INVALID;
	
        if (pAsyncSocketStream->IsRecvCompleted())
		{
			evType = SOCKET_EVENT_IN;
		}
	    
        if (pAsyncSocketStream->IsNeedToClose())
		{
			evType = SOCKET_EVENT_CLOSE;
		}

		if (evType != SOCKET_EVENT_INVALID)
		{
			spEventArray[nEventCount].m_nEventType = evType;
			spEventArray[nEventCount].m_pAsyncSocketStream = pAsyncSocketStream;
			nEventCount++;
		}
	}
	// 保存当前等待处理节点位置，下次优先处理
	m_nLastWaitToProcessPos = std::distance(iterBegin, iterNext);
	return TRUE;
}

VOID AsyncSocketStreamQueue::BroadcastMsg(SPDynamicBuffer &spBuffer)
{
	INT nErrorCode = 0;
	BOOL bRetCode = FALSE;
	CHECK_RETURN_VOID_QUIET(spBuffer);
	PAsyncSocketStream pAsyncSocketStream = NULL;
	VecPAsyncSocketStream::iterator iterCur = m_vecSocketStream.begin();
	VecPAsyncSocketStream::iterator iterEnd = m_vecSocketStream.end();
	VecPAsyncSocketStream::iterator iterTmp = iterCur;

	while (iterCur != iterEnd)
	{
		pAsyncSocketStream = (*iterCur);
		bRetCode = pAsyncSocketStream->Send(spBuffer, &nErrorCode);
		if (!bRetCode)
		{
			m_vecSocketStream.erase(iterCur);
			g_SafelyDeletePtr(pAsyncSocketStream);
		}
		iterCur++;
	}								
}


