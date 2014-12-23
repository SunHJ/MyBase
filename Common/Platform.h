#ifndef __PLATFORM_H__
#define __PLATFORM_H__

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
#include <string>
#include <assert.h>
#include <ctype.h>
#include <algorithm>
#include <vector>

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN			 // ������MFC�����ģ�顣�ڱ�������ʱ���Լ���������ɵ�һЩ�������õ�ģ��ʱ���ٶȸ��죬������С��
	//#include <WinSock2.h>
	#include <windows.h>
	#undef WIN32_LEAN_AND_MEAN
#else
	#include <unistd.h>
#endif // WIN32


#endif