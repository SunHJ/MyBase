#ifndef __NET_SOCKET_STREAM_H__
#define __NET_SOCKET_STREAM_H__

#include "Type.h"
#include "SocketWrapper.h"	   
#include "SocketStreamBuffer.h"

#define MAX_NET_PACKAGE_SIZE 65500

class SocketStream : private UnCopyable
{
private: 
	SOCKET m_hRemoteSocket;         // client socket
	STRING m_strRemoteIp;           // client ip
	USHORT m_usRemotePort;          // client port

public:
	SocketStream();
	virtual ~SocketStream();

public:
	BOOL Init(SOCKET hRemoteSocket, STRING strRemoteIp = "", USHORT usRemotePort = 0);
	BOOL Close();
	BOOL UnInit();

	STRING GetRemoteIp() CONST;
	USHORT GetRemotePort() CONST;
	SOCKET GetSocket() CONST;

	virtual INT Recv(SPDynamicBuffer &spBuffer, INT *pErrorCode = NULL);
	virtual INT Send(SPDynamicBuffer &spBuffer, INT *pErrorCode = NULL);
};

typedef SocketStream *				PSocketStream;
typedef SharedPtr<SocketStream>		SPSocketStream;		   
typedef std::vector<SPSocketStream> SPSocketStreamVector;

class AsyncSocketStream : private UnCopyable
{
public:
	AsyncSocketStream();
	~AsyncSocketStream();

	BOOL Init(SOCKET& hRemoteSocket, STRING strRemoteIp, USHORT usRemotePort, INT nMaxRecvSizePerSocket = MAX_NET_PACKAGE_SIZE);
	BOOL Close();
	BOOL UnInit();

	SOCKET GetSocket() CONST;
	STRING GetRemoteIp() CONST;
	USHORT GetRemotePort() CONST;

	BOOL IsNeedToClose();
	BOOL IsRecvCompleted();
	BOOL Send(SPDynamicBuffer &spBuffer, INT *pErrorCode /* = NULL */);
	BOOL Recv(SPDynamicBuffer &spBuffer, INT *pErrorCode /* = NULL */);

#ifdef PLATFORM_OS_WINDOWS 
	INT TryToActiveNextRecv();
	INT OnRecvCompleted(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);		
public:
	WSAOVERLAPPED m_wsaOverlapped;  // used in iocp
private:
	WSABUF	m_wsaBuffer;	// used in WSARecv	   
#else
public:
	INT TryEpollRecv();
private:
#endif // PLATFORM_OS_WINDOWS  
private:
	INT						m_nRecvSize;
	INT						m_nRecvErrorCode;
	BOOL					m_bNeedToCloseFlag;
	BOOL					m_bRecvCompletedFlag;				
	STRING					m_strRemoteIp;
	USHORT					m_usRemotePort;
	SOCKET					m_hRemoteSocket;
	SPSocketStreamBuffer	m_spDataBuffer;	 
};
typedef AsyncSocketStream *			PAsyncSocketStream;
typedef SharedPtr<AsyncSocketStream>	SPAsyncSocketStream;
typedef std::vector<PAsyncSocketStream> VecPAsyncSocketStream;

#endif //__NET_SOCKET_STREAM_H__
