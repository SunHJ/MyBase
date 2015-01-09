#ifndef __NET_SOCKET_ACCEPTOR_H__
#define __NET_SOCKET_ACCEPTOR_H__

/*#include "Type.h"*/
#include "SharedPtr.h"
#include "SocketEvent.h"
#include "SocketWrapper.h"
#include "SocketStream.h"

// 1  : indicates sucess
// 0  : non waiting connection or operations will complete later
// -1 : accept failed. this indicates a serious system error
extern inline INT g_AcceptToAsyncSocketStream(
	SOCKET hListenSocket,                           // listen socket
	PAsyncSocketStream &pAsyncSocketStream,         // async socket stream pointer
	INT *pErrorCode = NULL                          // error code
	);

// use smart pointer
extern inline INT g_AcceptToAsyncSocketStream(
	SOCKET hListenSocket,                           // listen socket
	SPAsyncSocketStream spAsyncSocketStream,        // async socket stream pointer
	INT *pErrorCode = NULL                          // error code
	);

class NonBlockSocketAcceptor : private UnCopyable
{
private:
	SOCKET m_hListenSocket;
	STRING m_strIpAddress;
	USHORT m_usPort;

	SPAsyncSocketStreamQueue m_spSocketStreamQueue;

public:
	NonBlockSocketAcceptor();
	~NonBlockSocketAcceptor();

	BOOL Init(
		CONST STRING &strIpAddress,
		CONST USHORT &usPort
		);

	BOOL AttachSocketStreamQueue(SPAsyncSocketStreamQueue &spQueue);

	BOOL WaitAccept(
		INT nMaxEventCount,
		INT &nEventCount,
		SPAsyncSocketEventArray spEventArray
		);

	BOOL UnInit();

private:
	BOOL WaitProcessAccept(
		INT nMaxEventCount,
		INT &nEventCount,
		SPAsyncSocketEventArray spEventArray
		);
};

typedef NonBlockSocketAcceptor * PNonBlockSocketAcceptor;
typedef SharedPtr<NonBlockSocketAcceptor> SPNonBlockSocketAcceptor;

#endif //__NET_SOCKET_ACCEPTOR_H__