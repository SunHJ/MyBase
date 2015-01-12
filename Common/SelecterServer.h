#ifndef __NET_SELECTER_SERVER_H__
#define __NET_SELECTER_SERVER_H__
#include "Buffer.h"				 
#include "Thread.h"		
#include "Semaphore.h"
#include "SocketEvent.h"
#include "SocketAcceptor.h"

class SelecterServer
{
public:
	SelecterServer();
	~SelecterServer();
	BOOL Start(STRING szIp, USHORT uPort);
	VOID Stop();

	virtual INT GetClientCount();
	virtual VOID ProcessNewConnect(SPAsyncSocketStream &spSocketStream);
	virtual VOID ProcessNetMessage(SPAsyncSocketStream &spSocketStream);
	virtual VOID ProcessClentClose(SPAsyncSocketStream &spSocketStream);

private:
	static VOID WorkThreadFun(void* pParam);

	VOID MainLoop();

private:
	BOOL m_bStop;
	Semaphore		m_SemStop;
	SimpleThread	m_cThread;
	SPNonBlockSocketAcceptor	m_spSocketAcceptor;
	SPAsyncSocketStreamQueue	m_spConnectSocket;
	SPAsyncSocketEventArray		m_spEventArray;  
	SPDynamicBuffer				m_spBuffer;
};

#endif //__NET_SELECTER_SERVER_H__