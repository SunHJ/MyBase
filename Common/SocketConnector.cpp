#include "SocketConnector.h"

SocketConnector::SocketConnector()
{
	m_nRecvBufSizePerSocket = 0;
	m_nSendBufSizePerSocket = 0;
	g_ZeroMemory(&m_BindLoaclAddr, sizeof(m_BindLoaclAddr));
	m_BindLoaclAddr.sin_family		= AF_INET;
	m_BindLoaclAddr.sin_addr.s_addr = INADDR_ANY;
	m_BindLoaclAddr.sin_port		= ::htons(0);	
}

/*
*/

INT SocketConnector::BindLocalIP(CPCCHAR cpcIPAddress, INT nPort /*= 0*/)
{
	INT nResult = 0;
	INT nRetCode = FALSE;
	ULONG ulAddress = INADDR_ANY;

	if ((cpcIPAddress) && (cpcIPAddress[0] != '\0'))
	{
		ulAddress = ::inet_addr(cpcIPAddress);
		if (ulAddress == INADDR_NONE)
			ulAddress = INADDR_ANY;
	}

	m_BindLoaclAddr.sin_addr.s_addr = ulAddress;
	m_BindLoaclAddr.sin_port = ::htons(nPort);

	return nResult;
}

PSocketStream SocketConnector::Connect(CPCCHAR cpcIpAddress, INT nPort)
{
	INT nRetCode;
	SOCKET sSocket;
	PSocketStream pReSocketStream = NULL;

	PROCESS_ERROR(cpcIpAddress);

	sSocket = g_ConnectServerSocket(cpcIpAddress, nPort, &nRetCode);
	PROCESS_ERROR_QUIET(0 == nRetCode);

	pReSocketStream = new SocketStream();
	PROCESS_ERROR(pReSocketStream);

	nRetCode = pReSocketStream->Init(sSocket, cpcIpAddress, nPort);
	PROCESS_ERROR(nRetCode);

Exit0:
	return pReSocketStream;
}