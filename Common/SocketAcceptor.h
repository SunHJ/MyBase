#ifndef __NET_SOCKET_ACCEPTOR_H__
#define __NET_SOCKET_ACCEPTOR_H__

/*#include "Type.h"*/
#include "SharedPtr.h"
#include "SocketEvent.h"
#include "SocketWrapper.h"
#include "SocketStreamQueue.h"

class NonBlockSocketAcceptor : private UnCopyable
{
private:
	INT		m_nEpollHandle;
	SOCKET	m_hListenSocket;
	STRING	m_strIpAddress;
	USHORT	m_usPort;

	SPAsyncSocketStreamQueue m_spSocketStreamQueue;

#ifdef PLATFORM_OS_LINUX
	VecSocketEvent	m_SocketEventVector;
#endif // PLATFORM_OS_LINUX		

public:
	NonBlockSocketAcceptor();
	~NonBlockSocketAcceptor();

	BOOL Init(CONST STRING &strIpAddress, CONST USHORT &usPort);
	BOOL Wait(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray);
	BOOL AttachSocketStreamQueue(SPAsyncSocketStreamQueue &spQueue);
	BOOL UnInit();

	USHORT GetServerPort() CONST{ return m_usPort; }

private:
	BOOL WaitProcessAccept(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray);
	BOOL WaitClientRequet(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray);

	BOOL _AcceptToAsyncSocketStream(PAsyncSocketStream &pAsyncSocketStream, INT *pErrorCode /* = NULL */);
// #ifdef PLATFORM_OS_WINDOWS
// 	BOOL _WaitProcessOrDestory(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray);
// #else
// 	BOOL _WaitProcessRecv(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray);
// #endif // PLATFORM_OS_WINDOWS
};

typedef NonBlockSocketAcceptor * PNonBlockSocketAcceptor;
typedef SharedPtr<NonBlockSocketAcceptor> SPNonBlockSocketAcceptor;

#endif //__NET_SOCKET_ACCEPTOR_H__
