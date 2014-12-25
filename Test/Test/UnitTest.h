#ifndef __TEST_UNITTEST_H__
#define __TEST_UNITTEST_H__
#include "Common.h"

class MyThreadClass : public Thread
{
public:
	MyThreadClass();
	~MyThreadClass();

private:

	virtual UINT Run();
};

extern void Test_PlatFormInfo();

extern void Test_Macro();

extern void Test_GlobalFun();

extern void Test_Interlocked();

extern int Test_Thread();


#endif