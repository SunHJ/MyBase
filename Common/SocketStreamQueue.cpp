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

BOOL AsyncSocketStreamQueue::AddClient(SPAsyncSocketStream &spAsyncSocketStream)
{
	m_vecSocketStream.push_back(spAsyncSocketStream);
	return TRUE;
}

BOOL AsyncSocketStreamQueue::DelClient(SPAsyncSocketStream &spAsyncSocketStream)
{
	VecSPAsyncSocketStream::iterator iterBegin = m_vecSocketStream.begin();
	VecSPAsyncSocketStream::iterator iterEnd = m_vecSocketStream.end();
	VecSPAsyncSocketStream::iterator iter = std::find(iterBegin, iterEnd, spAsyncSocketStream);
	if (iter != iterEnd)
		m_vecSocketStream.erase(iter);
	return TRUE;
}

VOID AsyncSocketStreamQueue::CloseAll()
{
	VecSPAsyncSocketStream::iterator iterBegin = m_vecSocketStream.begin();
	VecSPAsyncSocketStream::iterator iterEnd = m_vecSocketStream.end();

	SPAsyncSocketStream spSocketStream;
	while (iterBegin != iterEnd)
	{
		(*iterBegin)->Close();
		iterBegin++;
	}
	m_vecSocketStream.clear();
}

size_t AsyncSocketStreamQueue::GetCurStreamVectorLen() CONST
{
	return m_vecSocketStream.size();
}

BOOL AsyncSocketStreamQueue::Wait(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray, INT nEpollHandle/* = -1*/)
{
	BOOL bRetCode = FALSE;
#ifdef PLATFORM_OS_WINDOWS
	bRetCode = _WaitProcessRecvOrClose(nMaxEventCount, nEventCount, spEventArray);
#else
	bRetCode = _WaitProcessRecv(nMaxEventCount, nEventCount, spEventArray, nEpollHandle);
#endif // PLATFORM_OS_WINDOWS		  
	PROCESS_ERROR(bRetCode);
	bRetCode = TRUE;
Exit0:
	return bRetCode;
}

#ifdef PLATFORM_OS_WINDOWS
BOOL AsyncSocketStreamQueue::_WaitProcessRecvOrClose(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray)
{
	CHECK_RETURN_BOOL(nMaxEventCount > 0 && spEventArray && nEventCount <= nMaxEventCount);

	INT nReCode = 0;
	timeval tmTimeOut;
	tmTimeOut.tv_sec = 0;
	tmTimeOut.tv_usec = 0;
	size_t nStreamVectorLen = m_vecSocketStream.size();
	if (m_nLastWaitToProcessPos >= nStreamVectorLen)
		m_nLastWaitToProcessPos = 0;

	VecSPAsyncSocketStream::iterator iterBegin = m_vecSocketStream.begin();
	VecSPAsyncSocketStream::iterator iterEnd = m_vecSocketStream.end();
	VecSPAsyncSocketStream::iterator iterNext = iterBegin;
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

		
		VecSPAsyncSocketStream::iterator iterTemp = iterNext++;
		SPAsyncSocketStream spAsyncSocketStream = *iterTemp;
		ASSERT(spAsyncSocketStream);

		evType = SOCKET_EVENT_INVALID;
		if (spAsyncSocketStream->IsNeedToClose())
		{
			evType = SOCKET_EVENT_CLOSE;
		}
		if (spAsyncSocketStream->IsRecvCompleted())
		{
			evType = SOCKET_EVENT_IN;
		}
		if (evType != SOCKET_EVENT_INVALID)
		{
			spEventArray[nEventCount].m_nEventType = evType;
			spEventArray[nEventCount].m_spAsyncSocketStream = spAsyncSocketStream;
			nEventCount++;
		}
	}
	// 保存当前等待处理节点位置，下次优先处理
	m_nLastWaitToProcessPos = std::distance(iterBegin, iterNext);
	return TRUE;
}	
#else	 
BOOL AsyncSocketStreamQueue::_WaitProcessRecv(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray, INT nEpollHandle /*= -1*/)
{
	BOOL bResult = FALSE;
	INT nRetCode = 0, nRemainEventCount = 0;	
	struct epoll_event* pEpollEvent = NULL;

}
#endif // PLATFORM_OS_WINDOWS



