#include "UnitTest.h"

MyThreadClass::MyThreadClass()
{
}

MyThreadClass::~MyThreadClass()
{
}

UINT MyThreadClass::Run()
{
	static UINT uCount = 0;
	while (true)
	{
		uCount++;
		printf("%d\n", uCount);
		g_Sleep(1000);
	}
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
		lValue = g_InterlockedIncrement(&lValue);
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
			hThread.Stop();
			break;
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
