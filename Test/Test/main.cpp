#include "UnitTest.h"

#ifdef WIN32
#include <crtdbg.h>
#endif

template <class T>
class TestSize
{
private:
    typedef VOID (*Dtr)(T *&p);

    T m_Value;

public:
    TestSize()
    {
        printf("DtrSzie = %ld\n", sizeof(Dtr));
        printf("PVOIDSize = %ld\n", sizeof(PVOID));
    }
};

int main(int argc, char* argv[])
{
#ifdef WIN32
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
//  	Test_PlatFormInfo();
//  	Test_Macro();
//  	Test_Interlocked();
//  	Test_GlobalFun();
// 
// 	Test_Thread();
// 	Test_File();
// 
// 	Test_Pointer(); 
//     TestSize<int> test;
//	Test_Buffer();
	Test_Net();
	return 0;
}
