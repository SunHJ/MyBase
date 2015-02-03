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

#ifdef PLATFORM_OS_WINDOWS
class AsyncSocketStream : private UnCopyable
{
public:
	AsyncSocketStream();
	~AsyncSocketStream();

	BOOL Init(SOCKET& hRemoteSocket, STRING strRemoteIp, USHORT usRemotePort, INT nMaxRecvSizePerSocket = MAX_NET_PACKAGE_SIZE);
	BOOL Close();
	BOOL UnInit();

	STRING GetRemoteIp() CONST;
	USHORT GetRemotePort() CONST;

	BOOL IsNeedToClose();
	BOOL IsRecvCompleted();
	BOOL Send(SPDynamicBuffer &spBuffer, INT *pErrorCode /* = NULL */);
	BOOL Recv(SPDynamicBuffer &spBuffer, INT *pErrorCode /* = NULL */);

#ifdef PLATFORM_OS_WINDOWS 
	INT TryToActiveNextRecv();
// 	INT OnWaitNotifyed();
// 	INT GetWaitNotifyFlag();
// 	INT GetDelayDestoryFlag();
// 	INT GetRecvCompletedFlag();
	INT OnRecvCompleted(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);		

public:
	WSAOVERLAPPED m_wsaOverlapped;  // used in iocp	  

private:
	INT		m_nRecvCompletedSize;
	INT		m_nRecvCompletedErrorCode;
	DWORD   m_dwWsFlag;
	WSABUF	m_wsaBuffer;	// used in WSARecv	   
#else

#endif // PLATFORM_OS_WINDOWS  
private:
	SOCKET					m_hRemoteSocket;         // client socket
	STRING					m_strRemoteIp;           // client ip
	USHORT					m_usRemotePort;          // client port	
	BOOL					m_bNeedToCloseFlag;
	BOOL					m_bRecvCompletedFlag;
	SPSocketStreamBuffer	m_spDataBuffer;			 // 	 
};
typedef AsyncSocketStream *			PAsyncSocketStream;
typedef SharedPtr<AsyncSocketStream>	SPAsyncSocketStream;
typedef std::vector<SPAsyncSocketStream> VecSPAsyncSocketStream;
#else
typedef std::vector<struct epoll_event> VecSocketEvent;
#endif // PLATFORM_OS_WINDOWS

#endif //__NET_SOCKET_STREAM_H__
