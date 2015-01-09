#ifndef __STRUCTURE_FRAME_CONTROL__
#define __STRUCTURE_FRAME_CONTROL__

#include "Type.h"

class Timer
{
public:
	Timer();
	~Timer();

	VOID  Start();
	VOID  Stop();
	DWORD GetElapseInSec() CONST;
	DWORD GetElapseInMilliSec() CONST;

private:
#ifdef PLATFORM_OS_WINDOWS
	LARGE_INTEGER m_liFrequency;
	LARGE_INTEGER m_liStartTime;
	LARGE_INTEGER m_liStopTime;
#else
	timeval m_tvStartTime;
	timeval m_tvStopTime;
#endif

};


class FrameControl
{
public:
	FrameControl(INT nFPS = 18);
	~FrameControl();

	VOID Monitor();
private:
	INT m_nFPS;

};
#endif // !__STRUCTURE_FRAME_CONTROL__
