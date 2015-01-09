#ifndef __COMMON_NET_SOCKET_WRAPPER_H_
#define __COMMON_NET_SOCKET_WRAPPER_H_

#include "Type.h"

#ifdef PLATFORM_OS_WINDOWS
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#else 
#include <sys/types.h>          // for socket
#include <sys/socket.h>         // for socket 	  
#include <netinet/in.h>         // for sockaddr_in
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

extern inline INT		g_GetSocketLastError();
extern inline INT		g_IsSocketCanRestore();
extern inline INT		g_IsSocketWouldBlock();
extern inline INT		g_CheckCanRecv(SOCKET nSocket, CONST timeval *pcTimeout);
extern inline INT		g_CheckCanSend(SOCKET nSocket, CONST timeval *pcTimeout);

extern inline BOOL		g_CloseSocket(SOCKET &s);
extern inline BOOL		g_SetSocketNonBlock(SOCKET &s, INT *pErrorCode = NULL);
extern inline BOOL		g_SetSocketRecvTimeout(SOCKET &s, DWORD dwMilliSeconds, INT *pErrorCode = NULL);
extern inline BOOL		g_SetSocketSendTimeout(SOCKET &s, DWORD dwMilliSeconds, INT *pErrorCode = NULL);

extern inline SOCKET	g_CreateTCPSocket();
extern inline SOCKET	g_CreateUDPSocket();	  
extern inline SOCKET	g_CreateListenSocket(CONST CHAR *cszIP, USHORT usPort, INT *pErrorCode = NULL);
extern inline SOCKET	g_ConnectServerSocket(IN CPCCHAR cpcHostIP, IN USHORT usPort, INT *pErrorCode = NULL);

#endif