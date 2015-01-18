#include "SocketEvent.h"

AsyncSocketEvent::AsyncSocketEvent() : m_nEventType(ASYNC_SOCKET_EVENT_INVALID), m_spAsyncSocketStream()
{
	//
}

AsyncSocketEvent::~AsyncSocketEvent()
{
	//
}

VOID AsyncSocketEvent::Reset()
{
	m_nEventType = ASYNC_SOCKET_EVENT_INVALID;
	m_spAsyncSocketStream = SPAsyncSocketStream();
}


// StreamQueue 
AsyncSocketStreamQueue::AsyncSocketStreamQueue() : m_nLastWaitToProcessPos(0)
{
	m_vecAsyncSocketStream.clear();
}

AsyncSocketStreamQueue::~AsyncSocketStreamQueue()
{
	//
}

BOOL AsyncSocketStreamQueue::Wait(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray)
{
	BOOL bRetCode = FALSE;
	bRetCode = WaitProcessRecvOrClose(nMaxEventCount, nEventCount, spEventArray);
	PROCESS_ERROR(bRetCode);
	return TRUE;
Exit0:
	return FALSE;
}

BOOL AsyncSocketStreamQueue::SelectWait(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray)
{
	CHECK_RETURN_BOOL(nMaxEventCount > 0 && spEventArray && nEventCount <= nMaxEventCount);

	INT nReCode = 0;
	timeval tmTimeOut;
    tmTimeOut.tv_sec = 0;
    tmTimeOut.tv_usec = 0;
	size_t nStreamVectorLen = m_vecAsyncSocketStream.size();
	if (m_nLastWaitToProcessPos >= nStreamVectorLen)
		m_nLastWaitToProcessPos = 0;

	SP_ASYNC_STREAM_VECTOR::iterator iterBegin = m_vecAsyncSocketStream.begin();
	SP_ASYNC_STREAM_VECTOR::iterator iterEnd = m_vecAsyncSocketStream.end();
	SP_ASYNC_STREAM_VECTOR::iterator iterNext = iterBegin;
	std::advance(iterNext, m_nLastWaitToProcessPos);

	size_t nToProcessEventCount = nStreamVectorLen;
	while (nToProcessEventCount--)
	{
		if (iterNext == iterEnd)
			iterNext = iterBegin;

		SP_ASYNC_STREAM_VECTOR::iterator iterTemp = iterNext++;

		SPAsyncSocketStream spAsyncSocketStream = *iterTemp;
		nReCode = g_SelectDataIn(spAsyncSocketStream->GetSocket(), &tmTimeOut);
		if (1 == nReCode)
		{
			spEventArray[nEventCount].m_nEventType = ASYNC_SOCKET_EVENT_IN;
			spEventArray[nEventCount].m_spAsyncSocketStream = spAsyncSocketStream;
			nEventCount++;
		}
		else if (-1 == nReCode)
		{
			nReCode = g_GetSocketLastError();
			if (SOCKET_ERROR == nReCode)
			{
				spEventArray[nEventCount].m_nEventType = ASYNC_SOCKET_EVENT_CLOSE;
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

BOOL AsyncSocketStreamQueue::PushBack(SPAsyncSocketStream &spAsyncSocketStream)
{
	m_vecAsyncSocketStream.push_back(spAsyncSocketStream);
	return TRUE;
}

BOOL AsyncSocketStreamQueue::Remove(SPAsyncSocketStream &spAsyncSocketStream)
{
	SP_ASYNC_STREAM_VECTOR::iterator iterBegin = m_vecAsyncSocketStream.begin();
	SP_ASYNC_STREAM_VECTOR::iterator iterEnd = m_vecAsyncSocketStream.end();
	SP_ASYNC_STREAM_VECTOR::iterator iter = std::find(iterBegin, iterEnd, spAsyncSocketStream);

	if (iter != iterEnd)
		m_vecAsyncSocketStream.erase(iter);

	return TRUE;
}

VOID AsyncSocketStreamQueue::CloseAll()
{
	SP_ASYNC_STREAM_VECTOR::iterator iterBegin = m_vecAsyncSocketStream.begin();
	while (iterBegin != m_vecAsyncSocketStream.end())
	{
		(*iterBegin)->UnInit();
		iterBegin++;
	}
	m_vecAsyncSocketStream.clear();
}

BOOL AsyncSocketStreamQueue::WaitProcessRecvOrClose(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray)
{
	CHECK_RETURN_BOOL(nMaxEventCount > 0 && spEventArray && nEventCount <= nMaxEventCount);

	size_t nStreamVectorLen = m_vecAsyncSocketStream.size();
	if (m_nLastWaitToProcessPos >= nStreamVectorLen)
		m_nLastWaitToProcessPos = 0;
	SP_ASYNC_STREAM_VECTOR::iterator iterBegin = m_vecAsyncSocketStream.begin();
	SP_ASYNC_STREAM_VECTOR::iterator iterEnd = m_vecAsyncSocketStream.end();
	SP_ASYNC_STREAM_VECTOR::iterator iterNext = iterBegin;
	std::advance(iterNext, m_nLastWaitToProcessPos);

	size_t nToProcessEventCount = nStreamVectorLen;
	while (nToProcessEventCount--)
	{
		if (iterNext == iterEnd)
			iterNext = iterBegin;
		SP_ASYNC_STREAM_VECTOR::iterator iterTemp = iterNext++;

		SPAsyncSocketStream spAsyncSocketStream = *iterTemp;
		ASSERT(spAsyncSocketStream);

		// 		if (spAsyncSocketStream->IsStreamNeedToClose())
		// 		{
		// 			iterNext = m_vecAsyncSocketStream.erase(iterTemp);
		// 			iterBegin = m_vecAsyncSocketStream.begin();
		// 			iterEnd = m_vecAsyncSocketStream.end();
		// 			spAsyncSocketStream->UnInit();
		// 			continue;
		// 		}
		// 
		// 		if (!spAsyncSocketStream->IsIOCPRecvCompleted())
		// 			continue;

		if (nEventCount >= nMaxEventCount)
			continue;

		spEventArray[nEventCount].m_nEventType = ASYNC_SOCKET_EVENT_IN;
		spEventArray[nEventCount].m_spAsyncSocketStream = spAsyncSocketStream;
		nEventCount++;
	}

	m_nLastWaitToProcessPos = std::distance(iterBegin, iterNext);
	return TRUE;
}

size_t AsyncSocketStreamQueue::GetCurStreamVectorLen() CONST
{
	size_t uResult = 0;
	uResult = m_vecAsyncSocketStream.size();
	return uResult;
}
