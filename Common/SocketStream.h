#ifndef __NET_SOCKET_STREAM_H__
#define __NET_SOCKET_STREAM_H__

#include "Type.h"
#include "Buffer.h"
#include "SocketWrapper.h"

#define MAX_NET_PACKAGE_SIZE 65500

class AsyncSocketStream : private UnCopyable
{
private:
	SOCKET m_hRemoteSocket;         // client socket
	STRING m_strRemoteIp;           // client ip
	USHORT m_usRemotePort;          // client port

public:
	AsyncSocketStream();
	~AsyncSocketStream();

public:
	BOOL Init(SOCKET hRemoteSocket, STRING strRemoteIp = "", USHORT usRemotePort = 0);
	BOOL Close();
	BOOL UnInit();

	STRING GetRemoteIp() CONST;
	USHORT GetRemotePort() CONST;
	SOCKET GetSocket() CONST;

	INT Recv(SPDynamicBuffer &spBuffer, INT *pErrorCode = NULL);
	INT Send(SPDynamicBuffer &spBuffer, INT *pErrorCode = NULL);

};
typedef AsyncSocketStream *			PAsyncSocketStream;
typedef SharedPtr<AsyncSocketStream>	SPAsyncSocketStream;

#endif //__NET_SOCKET_STREAM_H__
