#ifndef __SUPER_SERVER_H__
#define __SUPER_SERVER_H__

#include "Type.h"
#include "Thread.h"
#include "SocketEvent.h"
#include "SocketStream.h"
#include "SocketAcceptor.h"		   

class SuperServer
{
private:
	SPAsyncSocketEventArray m_spEventArray;
	SPNonBlockSocketAcceptor m_spAcceptor;
	SPAsyncSocketStreamQueue m_spStreamQueue;

public:
	SuperServer();
	virtual ~SuperServer();

	BOOL Init(CONST STRING &strIp, CONST USHORT &usPort);
	VOID UnInit();

	BOOL Start();
	VOID Stop();

	VOID SetBroadcastMsg(CPCCHAR cpcMsg);
	virtual size_t GetClientCount();

private:
	BOOL Activate();
	BOOL ProcessNetEvent();
	BOOL ProcessAllCompletePackage(PAsyncSocketStream &spAsyncSocketStream);

	virtual VOID ProcessNewConnect(PAsyncSocketStream &spAsyncSocketStream);
	virtual VOID ProcessNetMessage(PAsyncSocketStream &spAsyncSocketStream, SPDynamicBuffer &spBuffer);
	virtual VOID ProcessClentClose(PAsyncSocketStream &spAsyncSocketStream);

private:
	BOOL			m_bLoopFlag;
	THREAD_ID		m_threadID;
	SPDynamicBuffer m_spDataBuffer;
	SPDynamicBuffer m_spBroadMsgBuffer;

#ifdef PLATFORM_OS_WINDOWS
	HANDLE m_hThread;
	static THREAD_FUNC_RET_TYPE WINAPI ThreadFunction(VOID *);
#else
	static THREAD_FUNC_RET_TYPE ThreadFunction(VOID *);
#endif
};
#endif // !__SUPER_SERVER_H__
