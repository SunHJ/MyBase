#include "SocketEvent.h"

SocketEvent::SocketEvent() : m_nEventType(SOCKET_EVENT_INVALID), m_spAsyncSocketStream()
{
	//
}

SocketEvent::~SocketEvent()
{
	//
}

VOID SocketEvent::Reset()
{
	m_nEventType = SOCKET_EVENT_INVALID;
	m_spAsyncSocketStream = SPSocketStream();
}

AsyncSocketEvent::AsyncSocketEvent() : m_nEventType(SOCKET_EVENT_INVALID), m_pAsyncSocketStream(NULL)
{

}

AsyncSocketEvent::~AsyncSocketEvent()
{

}

VOID AsyncSocketEvent::Reset()
{
	m_nEventType = SOCKET_EVENT_INVALID;
	m_pAsyncSocketStream = NULL;
}
