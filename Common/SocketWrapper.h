#ifndef __COMMON_NET_SOCKET_WRAPPER_H_
#define __COMMON_NET_SOCKET_WRAPPER_H_

#include "Type.h"

#ifdef PLATFORM_OS_WINDOWS
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#else 
#include <sys/poll.h>
#include <sys/types.h>          // for socket
#include <sys/ioctl.h>
#include <sys/socket.h>         // for socket
#include <netinet/in.h>         // for sockaddr_in
#include <arpa/inet.h>          // for inet_addr
#include <fcntl.h>              // for fcntl
#endif //PLATFORM_OS_WINDOWS

#define MAX_BACK_LOG_NUM 8

#ifdef PLATFORM_OS_WINDOWS
class NetService : UnCopyable
{
	DECLARE_SINGLETON_PATTERN(NetService);
public:
	~NetService ();

public:
	BOOL Strat(WORD wHighVersion = 2, WORD wLowVersion = 2);
	BOOL Stop();

private:
	NetService();

private:
	LONG m_lStarted;
};
#endif //PLATFORM_OS_WINDOWS

extern INT		g_GetSocketLastError();
extern INT		g_IsSocketCanRestore();
extern INT		g_IsSocketWouldBlock();
extern INT		g_CheckCanRecv(SOCKET nSocket, CONST timeval *pcTimeout);
extern INT		g_CheckCanSend(SOCKET nSocket, CONST timeval *pcTimeout);

extern BOOL		g_CloseSocket(SOCKET &s);
extern BOOL		g_SetSocketNonBlock(SOCKET &s, INT *pErrorCode = NULL);
extern BOOL		g_SetSocketRecvTimeout(SOCKET &s, DWORD dwMilliSeconds, INT *pErrorCode = NULL);
extern BOOL		g_SetSocketSendTimeout(SOCKET &s, DWORD dwMilliSeconds, INT *pErrorCode = NULL);

extern SOCKET	g_CreateTCPSocket();
extern SOCKET	g_CreateUDPSocket();	  
extern SOCKET	g_CreateListenSocket(CONST CHAR *cszIP, USHORT usPort, INT *pErrorCode = NULL);
extern SOCKET	g_ConnectServerSocket(CPCCHAR cpcHostIP, USHORT usPort, INT *pErrorCode = NULL);
extern INT g_SelectDataIn(SOCKET nSocket, CONST timeval *pcTimeouti = NULL);

#endif
