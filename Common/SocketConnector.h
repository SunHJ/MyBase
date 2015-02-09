#ifndef __NET_SOCKET_CONNECTOR_H__
#define __NET_SOCKET_CONNECTOR_H__

#include "Type.h"
#include "SocketStream.h"
#include "SocketWrapper.h"

class SocketConnector
{
private:
	INT m_nRecvBufSizePerSocket;
	INT m_nSendBufSizePerSocket;
	struct sockaddr_in m_BindLoaclAddr;

public:
	SocketConnector();

	INT BindLocalIP(CPCCHAR cpcIPAddress, INT nPort = 0);
	virtual PSocketStream Connect(CPCCHAR cpcIpAddress, INT nPort);
//	virtual PSocketStream ProxyConnect(CPCCHAR cpcProxyIP, INT nProxyPort,
//		CPCCHAR cpcProxyUserName, CPCCHAR cpcProxyPass, CPCCHAR cpcIP, INT nPort);

};

typedef SocketConnector *				PSocketConnector;
typedef SharedPtr<SocketConnector>		SPSocketConnector;

#endif //__NET_SOCKET_CONNECTOR_H__
