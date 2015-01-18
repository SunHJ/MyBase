#include "Platform.h"
#include "Global.h"
#include "SocketWrapper.h"

SOCKET g_CreateTCPSocket()
{
	SOCKET s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	PROCESS_ERROR(s > 0);
	return s;
Exit0:
	g_CloseSocket(s);
	return INVALID_SOCKET;
}

SOCKET g_CreateUDPSocket()
{
	SOCKET s = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	CHECK_RETURN_BOOL(s > 0);
	return INVALID_SOCKET;
}

SOCKET g_CreateListenSocket(CONST CHAR *cszIP, USHORT usPort, INT *pErrorCode)
{
	SOCKET nListenSocket = g_CreateTCPSocket();
	ULONG ulAddress = INADDR_ANY;
	int nOptVal = 1, nRetCode = 0;
	sockaddr_in saLocalAddr;

	PROCESS_ERROR(INVALID_SOCKET != nListenSocket);
	if (NULL != cszIP && cszIP[0] != '\0')
	{
		if (INADDR_NONE == (ulAddress = ::inet_addr(cszIP)))
		{
			ulAddress = INADDR_ANY;
		}
	}

	nRetCode = ::setsockopt(nListenSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&nOptVal, sizeof(nOptVal));
	PROCESS_ERROR(0 == nRetCode);

	saLocalAddr.sin_family = AF_INET;
	saLocalAddr.sin_addr.s_addr = ulAddress;
	saLocalAddr.sin_port = ::htons(usPort);

	nRetCode = ::bind(nListenSocket, (struct sockaddr *)&saLocalAddr, sizeof(saLocalAddr));
	PROCESS_ERROR(0 == nRetCode);

	nRetCode = ::listen(nListenSocket, MAX_BACK_LOG_NUM);
	PROCESS_ERROR(0 == nRetCode);
	return nListenSocket;

Exit0:
	g_SetErrorCode(pErrorCode, g_GetSocketLastError());
	g_CloseSocket(nListenSocket);
	return nListenSocket;
}

SOCKET g_ConnectServerSocket(CPCCHAR cpcHostIP, USHORT usPort, INT *pErrorCode /* = NULL */)
{
	SOCKET nReturnSocket = g_CreateTCPSocket();
	ULONG ulAddress = INADDR_ANY;
	INT nRetCode = 0;
    INT nOptVal = 1;
	sockaddr_in saServerAddr;

	PROCESS_ERROR(INVALID_SOCKET != nReturnSocket);
	if (NULL != cpcHostIP && cpcHostIP[0] != '\0')
	{
		ulAddress = ::inet_addr(cpcHostIP);
		if (INADDR_NONE == ulAddress)
		{
			ulAddress = INADDR_ANY;
		}
	}

	nRetCode = ::setsockopt(nReturnSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&nOptVal, sizeof(nOptVal));
	PROCESS_ERROR(0 == nRetCode);

	saServerAddr.sin_family = AF_INET;
	saServerAddr.sin_addr.s_addr = ulAddress;
	saServerAddr.sin_port = ::htons(usPort);

	nRetCode = ::connect(nReturnSocket, (struct sockaddr *) &saServerAddr, sizeof(struct sockaddr));

	PROCESS_ERROR(0 == nRetCode);

	return nReturnSocket;
Exit0:
	g_SetErrorCode(pErrorCode, g_GetSocketLastError());
	g_CloseSocket(nReturnSocket);
	return nReturnSocket;
}

// return -1: error, 0: timeout, 1: success
INT g_SelectDataIn(SOCKET nSocket, CONST timeval *pcTimeout)
{
	if (INVALID_SOCKET == nSocket)
		return -1;

	fd_set FDSet;
	FD_ZERO(&FDSet);
	FD_SET(nSocket, &FDSet);

	timeval  TempTimeout;
	timeval *pTempTimeout = NULL;

	if (pcTimeout)
	{
		TempTimeout = *pcTimeout;
		pTempTimeout = &TempTimeout;
	}

	int nRetCode = ::select(nSocket + 1, &FDSet, NULL, NULL, pTempTimeout);

	if (nRetCode == 0)
		return 0;

	if (nRetCode > 0)
		return 1;

	return -1;
}

//Windows 与 Linux 分别封装 
#ifdef PLATFORM_OS_WINDOWS

NetService::NetService() : m_lStarted(0)
{
	NetService::Strat();
}

NetService::~NetService()
{
	NetService::Stop();
}

BOOL NetService::Strat(WORD wHighVersion /* = 2 */, WORD wLowVersion /* = 2 */)
{
	INT nRetCode = 0;
	if (1 == g_AtomicIncrement(&m_lStarted))
	{
		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(wHighVersion, wLowVersion);
		nRetCode = ::WSAStartup(wVersionRequested, &wsaData);
		CHECK_RETURN_BOOL(0 == nRetCode);
	}
	else
		g_AtomicDecrement(&m_lStarted);
	return TRUE;
}

BOOL NetService::Stop()
{
	INT nRetCode = 0;
	if (0 == g_AtomicDecrement(&m_lStarted))
	{
		nRetCode = ::WSACleanup();
		CHECK_RETURN_BOOL(0 == nRetCode);
	}
	return TRUE;
}

INT g_GetSocketLastError()
{
	return ::WSAGetLastError();
}

INT g_IsSocketCanRestore()
{ 
	return (EINTR == ::WSAGetLastError());
}

INT g_IsSocketWouldBlock()
{
	// WSA_IO_PENDING : overlapped operations will complete later.
	// WSAEWOULDBLOCK : non waiting connection in listen-socket queue.
	INT nLastError = ::WSAGetLastError();
	return (
		(WSAEWOULDBLOCK == nLastError) ||
		(WSA_IO_PENDING == nLastError)
		);
}

// return -1: error, 0: timeout, 1: success
INT g_CheckCanRecv(SOCKET nSocket, CONST timeval *pcTimeout)
{
	if (INVALID_SOCKET == nSocket)
		return -1;

	fd_set FDSet;
	FD_ZERO(&FDSet);
	FD_SET(nSocket, &FDSet);

	timeval  TempTimeout;
	timeval *pTempTimeout = NULL;

	if (pcTimeout)
	{
		TempTimeout = *pcTimeout;
		pTempTimeout = &TempTimeout;
	}

	int nRetCode = ::select(nSocket + 1, &FDSet, NULL, NULL, pTempTimeout);

	if (nRetCode == 0)
		return 0;

	if (nRetCode > 0)
		return 1;

// 	nRetCode = g_GetSocketLastError();
	return -1;
}

// return -1: error, 0: timeout, 1: success
INT g_CheckCanSend(SOCKET nSocket, CONST timeval *pcTimeout)
{
	if (INVALID_SOCKET == nSocket)
		return -1;

	fd_set FDSet;
	FD_ZERO(&FDSet);
	FD_SET(nSocket, &FDSet);

	timeval  TempTimeout;
	timeval *pTempTimeout = NULL;

	if (pcTimeout)
	{
		TempTimeout = *pcTimeout;
		pTempTimeout = &TempTimeout;
	}

	int nRetCode = select(nSocket + 1, NULL, &FDSet, NULL, pTempTimeout);

	if (nRetCode == 0)
		return 0;

	if (nRetCode > 0)
		return 1;

	//nRetCode = g_GetSocketLastError();
	return -1;
}

BOOL g_CloseSocket(SOCKET &s)
{
	if (INVALID_SOCKET != s)
	{
		struct linger li;
		li.l_onoff  = 1;
		li.l_linger = 0;
		::setsockopt(s, SOL_SOCKET, SO_LINGER, (char *)&li, sizeof(li));

		CHECK_RETURN_BOOL(0 == ::closesocket(s));
		s = INVALID_SOCKET;
	}

	return TRUE;
}

BOOL g_SetSocketNonBlock(SOCKET &s, INT *pErrorCode)
{
	INT nRetCode = 0;
	ULONG ulOption = 1;

	nRetCode = ::ioctlsocket(s, FIONBIO, &ulOption);
	PROCESS_ERROR(0 == nRetCode);
	return TRUE;
Exit0:
	g_SetErrorCode(pErrorCode, g_GetSocketLastError());
	return FALSE;
}

BOOL g_SetSocketRecvTimeout(SOCKET &s, DWORD dwMilliSeconds, INT *pErrorCode)
{
	INT nTimeOut = static_cast<INT>(dwMilliSeconds);
	INT nRetCode = 0;
	nRetCode = ::setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (PCHAR)&nTimeOut, sizeof(INT));

	PROCESS_ERROR(0 == nRetCode);
	return TRUE;
Exit0:
	g_SetErrorCode(pErrorCode, g_GetSocketLastError());
	return FALSE;
}

BOOL g_SetSocketSendTimeout(SOCKET &s, DWORD dwMilliSeconds, INT *pErrorCode)
{
	INT nTimeOut = static_cast<INT>(dwMilliSeconds);
	INT nRetCode = 0;
	nRetCode = ::setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (PCHAR)&nTimeOut, sizeof(INT));

	PROCESS_ERROR(0 == nRetCode);
	return TRUE;
Exit0:
	g_SetErrorCode(pErrorCode, g_GetSocketLastError());
	return FALSE;
}

#else
INT g_GetSocketLastError()
{
	return errno;
}

INT g_IsSocketCanRestore()
{
	return (EINTR == errno);
}

INT g_IsSocketWouldBlock()
{
	return (
		(EAGAIN == errno) || 
		(EWOULDBLOCK == errno) );
}

// return -1: error, 0: timeout, 1: success
INT g_CheckCanRecv(SOCKET nSocket, CONST timeval *pcTimeout)
{
	if (INVALID_SOCKET == nSocket)
		return -1;

	int nTimeout = -1;
	if (pcTimeout)
	{
		nTimeout = (pcTimeout->tv_sec * 1000) + (pcTimeout->tv_usec / 1000);
	}

	struct pollfd PollFD;
	PollFD.fd = nSocket;
	PollFD.events  = POLLIN;
	PollFD.revents = 0;

	int nRetCode = ::poll(&PollFD, 1, nTimeout);

	if (nRetCode == 0)
		return 0;

	if (nRetCode > 0)
	{
		ASSERT(
			(PollFD.revents & POLLIN ) ||
			(PollFD.revents & POLLERR) ||
			(PollFD.revents & POLLHUP)
			);
		return 1;
	}

	// 	nRetCode = g_GetSocketLastError();
	return -1;
}

// return -1: error, 0: timeout, 1: success
INT g_CheckCanSend(SOCKET nSocket, CONST timeval *pcTimeout)
{
	if (INVALID_SOCKET == nSocket)
		return -1;

	int nTimeout = -1;
	if (pcTimeout)
	{
		nTimeout = (pcTimeout->tv_sec * 1000) + (pcTimeout->tv_usec / 1000);
	}

	struct pollfd PollFD;
	PollFD.fd = nSocket;
	PollFD.events  = POLLOUT;
	PollFD.revents = 0;

	int nRetCode = poll(&PollFD, 1, nTimeout);

	if (nRetCode == 0)
		return 0;

	if (nRetCode > 0)
	{
		ASSERT(
			(PollFD.revents & POLLOUT) ||
			(PollFD.revents & POLLERR) ||
			(PollFD.revents & POLLHUP)
			);
		return 1;
	}

	//nRetCode = g_GetSocketLastError();
	return -1;
}

BOOL g_CloseSocket(SOCKET &s)
{
	if (INVALID_SOCKET != s)
	{
        INT nRetCode = 0;
		struct linger li;
		li.l_onoff = 1;
		li.l_linger = 0;
		::setsockopt(s, SOL_SOCKET, SO_LINGER, (char *)&li, sizeof(li));
        nRetCode = ::close(s);
		CHECK_RETURN_BOOL(0 == nRetCode);
		s = INVALID_SOCKET;
	}

	return TRUE;
}

BOOL g_SetSocketNonBlock(SOCKET &s, INT *pErrorCode)
{ 
	int nRetCode = 0;
	int nOption  = 0;

	nOption = ::fcntl(s, F_GETFL, 0);
	nRetCode = ::fcntl(s, F_SETFL, nOption | O_NONBLOCK);
	PROCESS_ERROR(0 == nRetCode);

	return TRUE;
Exit0:
	g_SetErrorCode(pErrorCode, g_GetSocketLastError());
	return FALSE;
}

BOOL g_SetSocketRecvTimeout(SOCKET &s, DWORD dwMilliSeconds, INT *pErrorCode)
{
    INT nRetCode = 0;
	struct timeval sTimeOut;
	sTimeOut.tv_sec = dwMilliSeconds / 1000;                                // second
	sTimeOut.tv_usec = (dwMilliSeconds - sTimeOut.tv_sec * 1000) * 1000;    // microseconds
	nRetCode = ::setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (PCHAR)&sTimeOut, sizeof(struct timeval));
	PROCESS_ERROR(0 == nRetCode);

	return TRUE;
Exit0:
	g_SetErrorCode(pErrorCode, g_GetSocketLastError());
	return FALSE;
}

BOOL g_SetSocketSendTimeout(SOCKET &s, DWORD dwMilliSeconds, INT *pErrorCode)
{
    INT nRetCode = 0;
	struct timeval sTimeOut;
	sTimeOut.tv_sec = dwMilliSeconds / 1000;                                // second
	sTimeOut.tv_usec = (dwMilliSeconds - sTimeOut.tv_sec * 1000) * 1000;    // microseconds
	nRetCode = ::setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (PCHAR)&sTimeOut, sizeof(struct timeval));
	PROCESS_ERROR(0 == nRetCode);

	return TRUE;
Exit0:
	g_SetErrorCode(pErrorCode, g_GetSocketLastError());
	return FALSE;
}
#endif //PLATFORM_OS_WINDOWS
