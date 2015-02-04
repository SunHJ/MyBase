#ifndef __NET_SOCKET_ACCEPTOR_H__
#define __NET_SOCKET_ACCEPTOR_H__

#include "SharedPtr.h"	
#include "Semaphore.h"
#include "SocketEvent.h"
#include "SocketWrapper.h"
#include "SocketStreamQueue.h"

class NonBlockSocketAcceptor : private UnCopyable
{
private:
	STRING	m_strIpAddress;
	SOCKET	m_hListenSocket;
	USHORT	m_usPort;

	SPAsyncSocketStreamQueue m_spSocketStreamQueue;
public:
	NonBlockSocketAcceptor();
	~NonBlockSocketAcceptor();

	BOOL Init(CONST STRING &strIpAddress, CONST USHORT &usPort);
	BOOL Wait(INT &nEventCount, SPAsyncSocketEventArray spEventArray);
	BOOL AttachSocketStreamQueue(SPAsyncSocketStreamQueue &spQueue);
	BOOL UnInit();

	USHORT GetServerPort() CONST{ return m_usPort; }

private:
	BOOL WaitProcessAccept(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray);
	BOOL WaitClientRequet(INT nMaxEventCount, INT &nEventCount, SPAsyncSocketEventArray spEventArray);

	BOOL _AcceptToAsyncSocketStream(PAsyncSocketStream &pAsyncSocketStream, INT *pErrorCode /* = NULL */);

#ifdef PLATFORM_OS_LINUX   
public:
	VOID MainLoopRecv();

private:
	BOOL m_bLoopFlag;
	SimpleThread	m_cRecvThread;

	INT		m_nEpollHandle;
	BOOL _EpollWaitProcess(INT nMaxEventCount);		    
	struct epoll_event m_EpollEvents[MAX_SOCKET_EVENT];

	INT m_nHeadPos;
	INT m_nTailPos;
	Semaphore m_Semap;
	PAsyncSocketStream WaitQueue[MAX_SOCKET_EVENT];
#endif // PLATFORM_OS_LINUX
};

typedef NonBlockSocketAcceptor * PNonBlockSocketAcceptor;
typedef SharedPtr<NonBlockSocketAcceptor> SPNonBlockSocketAcceptor;

#endif //__NET_SOCKET_ACCEPTOR_H__
