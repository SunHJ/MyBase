#include "Common.h"

int main(int argc, char* argv[])
{
	DEBUG_PRINT_INT(1 + 2);
	printf("%s\n", PLATFORM);
	LONG lValue = 0;
	for (int i = 0; i < 10; i++)
	{
		lValue = g_InterlockedIncrement(&lValue);
		printf("%d\n", lValue);
	}
	return 0;
}
