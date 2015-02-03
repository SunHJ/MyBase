#ifndef __NET_SELECTER_SERVER_H__
#define __NET_SELECTER_SERVER_H__
#include "Buffer.h"				 
#include "Thread.h"		
#include "Semaphore.h"
#include "SocketAcceptor.h"
#include "SocketStreamQueue.h"

class SelecterServer
{
	static VOID WorkThreadFun(VOID* pParam);
public:
	SelecterServer();
	~SelecterServer();
	BOOL Init(CONST STRING strIpAddress, CONST USHORT usPort);
	VOID UnInit();
	BOOL Start();
	VOID Stop();

	virtual INT GetClientCount();
	virtual VOID ProcessNewConnect(SPSocketStream &spSocketStream);
	virtual VOID ProcessNetMessage(SPSocketStream &spSocketStream);
	virtual VOID ProcessClentClose(SPSocketStream &spSocketStream);

private:
	VOID MainLoop();
	BOOL _WaitClientAccept(INT nMaxEvenCount, INT &nEventCount, SPSocketEventArray spEventArray);

private:
	BOOL			m_bStop;
	Semaphore		m_SemStop;
	SimpleThread	m_cThread;		
	SPDynamicBuffer	m_spBuffer;

private:
	SOCKET				m_hListenSocket;
	STRING				m_strIpAddress;
	USHORT				m_usPort;
	SPSocketEventArray	m_spEventArray;
	SPSocketStreamQueue	m_spConnectSocket;
};

#endif //__NET_SELECTER_SERVER_H__