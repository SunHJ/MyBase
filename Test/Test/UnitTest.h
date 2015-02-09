#ifndef __TEST_UNITTEST_H__
#define __TEST_UNITTEST_H__
#include "Common.h"

class MyThreadClass : public IThread
{
public:
	MyThreadClass();
	~MyThreadClass();

private:
    UINT         m_nCount;
	virtual UINT Run();
};

extern void Test_PlatFormInfo();

extern void Test_Macro();

extern void Test_GlobalFun();

extern void Test_Interlocked();

extern int Test_Thread();

extern void Test_File();

extern void Test_Pointer();

extern void Test_Buffer();

extern void Test_Net();

class Client
{
public:
	Client();
	~Client();
	BOOL Close();
	BOOL ConnectServer(CPCCHAR cpcIPAddress, INT nPort);
	VOID SetMsg2Server(CPCCHAR cpcMsg);

	VOID RecvMainLoop();
private:
	SocketConnector m_Connector;

	SPDynamicBuffer m_spRecvBuff;
	
	BOOL m_bConnect;
	PSocketStream m_pConnectSocket;

	BOOL m_bRecvLoop;
	SimpleThread m_Recv;
};
extern void Test_NetClient(CPCCHAR cpcIP, INT nPort);

#endif
