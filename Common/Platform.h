#ifndef __PUBLIC_PLATFORM_H__
#define __PUBLIC_PLATFORM_H__

/*----------------------------------- SYSTEM MACRO DEFINE -----------------------------------*/

#ifdef WIN32
	#define PLATFORM	"windows"
	#define PLATFORM_OS_WINDOWS
#else
	#define PLATFORM	"linux"
	#define PLATFORM_OS_LINUX

#endif // WIN32

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>	
#include <string>	
#include <map>	  
#include <list>
#include <vector>
#include <algorithm>

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN			 // ������MFC�����ģ�顣�ڱ�������ʱ���Լ���������ɵ�һЩ�������õ�ģ��ʱ���ٶȸ��죬������С��
	//#include <WinSock2.h>
	#include <windows.h>   
	#include <process.h>
	#undef WIN32_LEAN_AND_MEAN
#else
    #include <string.h>
	#include <unistd.h>	
	#include <pthread.h>
    #include <semaphore.h>
	#include <sys/time.h>	
	#include <sys/stat.h>
    
	#define TRUE        1
    #define FALSE       0
    #define INFINITE    0xFFFFFFFF
    #define VOID        void
    #define CONST       const
#endif // WIN32

#endif	// __PUBLIC_PLATFORM_H__
