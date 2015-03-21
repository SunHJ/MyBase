#include "UnitTest.h"

MyThreadClass::MyThreadClass()
{
    m_nCount = 0;
}

MyThreadClass::~MyThreadClass()
{
}

UINT MyThreadClass::Run()
{
	m_nCount++;
	printf("%d\n", m_nCount);
	g_Sleep(1);
	return 0;
}

void Test_PlatFormInfo()
{
	printf("%s\n", PLATFORM);
}

void Test_Macro()
{
	DEBUG_PRINT_INT(1 + 2);
}

void Test_GlobalFun()
{
	char szStr[] = { 'a', 's', 'd', 'a', 's', 'd', 'c', 'c', 'c', '\0' };
	PCHAR pcPtr = szStr;
	printf("%s\n", szStr);
	g_TrimStingBothEnds(pcPtr, 'c');
	printf("%s\n", pcPtr);
}

void Test_Interlocked()
{
	LONG lValue = 0;
	for (int i = 0; i < 10; i++)
	{
		lValue = g_AtomicIncrement(&lValue);
		printf("%d\n", lValue);
	}
}
#include <iostream>
int Test_Thread()
{
	MyThreadClass hThread;
	hThread.Start();
	char cType = 0;
	while (true)
	{
		//scanf("%c", &cType);
		std::cin >> cType;
        if (cType == 'q')
        {
            break;
        }
        else if (cType == 'd')
		{
			hThread.Stop();
		}
		else if (cType == 's')
		{
			hThread.SuspendThread();
		}
		else if (cType == 'c')
		{
			hThread.ResumeThread();
		}
		else
		{
			printf("UnKnown CMD\n");
		}

	}
	return 0;
}

void Test_Pointer()
{
	struct MyStruct
	{
		int a;
		int b;
		int c;
	};
	MyStruct* pStruct = new MyStruct();
	SharedPtr<MyStruct> spStruct = SharedPtr<MyStruct>(pStruct);
	spStruct->a = 0;
	spStruct->b = 1;
	spStruct->c = 2;
}

void Test_File()
{
	File objFile;
	objFile.Open("test.txt", "w+");
	LONG lSize = objFile.Size();
	printf("Size = %d\n", lSize);
	LONG lPos = objFile.Tell();
	printf("CurPos = %d", lPos);
	objFile.Write("123456\n", 7, 7);
	lPos = objFile.Tell();
	printf("CurPos = %d\n", lPos);
	objFile.WriteTextLine("abcdefghijklmnopqrstuvwxyz", 26);
	objFile.WriteTextLine("abcdefghijklmnopqrs", 26);
	lPos = objFile.Tell();
	printf("CurPos = %d\n", lPos);

	BOOL bReCode = 0;
	CHAR szBuffer[256];
	LONG lReCode = 0;
	bReCode = objFile.SeekBegin(0);
	lReCode = objFile.ReadTextLine(szBuffer, 255);
	printf("ReadTextLine Result: %d %s", lReCode, szBuffer);
	lReCode = objFile.Read(szBuffer, 255, 10);
    szBuffer[10] = '\0';
	printf("Read Result: %d %s\n", lReCode, szBuffer);
}

void Test_Buffer()
{
	SPDynamicBuffer spBuffer(new DynamicBuffer());
	printf("BufferInfo Size:%d UsedSize:%d LeftSize:%d\n",
		spBuffer->GetTotalSize(), spBuffer->GetUsedSize(), spBuffer->GetLeftSize());

	CHAR szBuffer[] = "asahjkdhasjkdhajkshdjkalshd\0";
	spBuffer->InsertDataIntoHead(szBuffer, strlen(szBuffer));					
	printf("BufferInfo Size:%d UsedSize:%d LeftSize:%d\n",
		spBuffer->GetTotalSize(), spBuffer->GetUsedSize(), spBuffer->GetLeftSize());

	PVOID pBuffer = spBuffer->GetDataPtr(10);
	PCHAR pString = static_cast<PCHAR>(pBuffer);
	printf("%s\n", pString);
}

extern void Test_Net()
{
	//SelecterServer server;
	SuperServer server;
	server.Init("", 1234);
	server.Start();
	PCHAR pMsg = NULL;
	char szBuffer[257];

	while (true)
	{
		//scanf("%c", &cType);
		std::cin >> szBuffer;
		if (szBuffer[0] == 'q')
		{
			server.Stop();
			break;
		}
		else if (szBuffer[0] == 's')
		{
			server.Stop();
		}
		else if (szBuffer[0] == 'l')
        {
            printf(">>>>>Client Count %d\n", server.GetClientCount());
        }
		else if (szBuffer[0] == 'm')
		{
			pMsg = &szBuffer[1];
			server.SetBroadcastMsg(pMsg);
		}
		else
		{
			printf("UnKnown CMD\n");
		}

	}
}

VOID RecvThread(PVOID pParam)
{
	Client* pClient = (Client*)pParam;
	pClient->RecvMainLoop();
}

Client::Client()
{
#ifdef PLATFORM_OS_WINDOWS
	SINGLETON_GET_PTR(NetService)->Strat();
#endif	

	m_pConnectSocket = NULL;
	m_bConnect = FALSE;
	m_bRecvLoop = FALSE;
	m_spRecvBuff = SPDynamicBuffer(new DynamicBuffer(1024));
}

Client::~Client()
{
	m_bRecvLoop = FALSE;
	if (NULL != m_pConnectSocket)
	{
		m_pConnectSocket->Close();
		g_SafelyDeletePtr(m_pConnectSocket);
	}

#ifdef PLATFORM_OS_WINDOWS
	SINGLETON_GET_PTR(NetService)->Stop();
#endif
}

VOID Client::RecvMainLoop()
{
	INT nRetCode = 0, nErrorCode = 0;
	timeval tmTimeOut;
	tmTimeOut.tv_sec = 0;
	tmTimeOut.tv_usec = 0;

	m_bRecvLoop = TRUE;

	while (m_bRecvLoop)
	{
		nRetCode = g_CheckCanRecv(m_pConnectSocket->GetSocket(), &tmTimeOut);
		if (1 == nRetCode)
		{
			m_spRecvBuff->Reset();
			nRetCode = m_pConnectSocket->Recv(m_spRecvBuff, &nErrorCode);
			PROCESS_ERROR_QUIET(nRetCode > 0);
			printf("Recv %d Bytes from Server(%s)\n", m_spRecvBuff->GetUsedSize(), m_pConnectSocket->GetRemoteIp().c_str());
		}
		else if (-1 == nErrorCode)
		{
			PROCESS_ERROR_QUIET(FALSE);
		}
	}

Exit0:
	m_bRecvLoop = FALSE;
	printf("Client has closed.\n");
}

BOOL Client::Close()
{
	if (m_bRecvLoop)
	{
		m_bRecvLoop = FALSE;
		m_Recv.Stop();
	}

	if (m_bConnect)
	{
		m_pConnectSocket->Close();
	}					  
	return TRUE;
}


BOOL Client::ConnectServer(CPCCHAR cpcIPAddress, INT nPort)
{
	INT nErrorCode = 0;
	BOOL bResult = FALSE, bRetCode = FALSE;
	if (m_bRecvLoop)
	{
		m_bRecvLoop = FALSE;
		m_Recv.Stop();
		m_pConnectSocket->Close();
		g_SafelyDeletePtr(m_pConnectSocket);
	}
	m_pConnectSocket = m_Connector.Connect(cpcIPAddress, nPort);
	PROCESS_ERROR_QUIET(m_pConnectSocket);

	bRetCode = m_pConnectSocket->SetToNonBlock();
	PROCESS_ERROR_QUIET(bRetCode);
	
	m_bConnect = TRUE;

	bRetCode = m_Recv.Start(&RecvThread, this);
	PROCESS_ERROR_QUIET(bRetCode);

	bResult = TRUE;
Exit0:
	return bResult;
}

VOID Client::SetMsg2Server(CPCCHAR cpcMsg)
{
	INT nErrorCode = 0;
	if (m_bConnect)
	{
		SPDynamicBuffer spSendBuff = SPDynamicBuffer(new DynamicBuffer());
		spSendBuff->InsertDataIntoHead(cpcMsg, strlen(cpcMsg));
		m_pConnectSocket->Send(spSendBuff, &nErrorCode);
	}
	else
	{
		printf("Client can't connect to Server.\n");
	}
}

void Test_NetClient(CPCCHAR cpcIP, INT nPort)
{
	int nRetCode = 0;
	char* pMsg = NULL;
	char szBuffer[257];
	Client client;

	while (true)
	{
		//scanf("%c", &cType);
		std::cin >> szBuffer;
		if (szBuffer[0] == 'q')
		{
			client.Close();
			break;
		}
		else if (szBuffer[0] == 'c')
		{
			nRetCode = client.ConnectServer(cpcIP, nPort);
			if (FALSE == nRetCode)
			{
				printf("Connect %s:%d failed.\n", cpcIP, nPort);
			}
			else
			{
				printf("Connect %s:%d success.\n", cpcIP, nPort);
			}
		}
		else if (szBuffer[0] == 'm')
		{
			pMsg = &szBuffer[1];
			client.SetMsg2Server(pMsg);
		}
		else
		{
			printf("UnKnown CMD\n");
		}

	}
}

void Test_Heap()
{
	int nValue = 0, nRetCode;
	char szBuffer[257];
	Heap tHeap(20, FALSE);
	srand((size_t)time(NULL));
	for (int i = 0; i < 20; i++)
	{
		nValue = rand();
		tHeap.Heap_Push(nValue);
	}


	while (true)
	{
		gets(szBuffer);
		if (szBuffer[0] == 'q')
		{
			break;
		}
		else if (szBuffer[0] == 'l')
		{
			tHeap.PrintHeap();
		}
		else if (szBuffer[0] == 'i')
		{
			nValue = atoi(&szBuffer[2]);
			nRetCode = tHeap.Heap_Push(nValue);
			printf("Push Result : %d\n", nRetCode);
		}
		else if (szBuffer[0] == 'd')
		{
			nValue = atoi(&szBuffer[2]);
			nRetCode = tHeap.Heap_Pop(nValue);
			printf("Pop Result : %d\n", nRetCode);
		}
		else if (szBuffer[0] == 'f')
		{
			nValue = atoi(&szBuffer[2]);
			nRetCode = tHeap.HasValue(nValue);
			printf("Find Result : %d\n", nRetCode);	   
		}
		else if (szBuffer[0] == '0')
		{
			nRetCode = tHeap.FindMin();
			printf("Find Result : %d\n", nRetCode);
		}
		else if (szBuffer[0] == '1')
		{
			nRetCode = tHeap.FindMax();
			printf("Find Result : %d\n", nRetCode);
		}
		else
		{
			printf("UnKnown CMD\n");
		}	 
	}

}
