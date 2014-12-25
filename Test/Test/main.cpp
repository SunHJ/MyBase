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
	char szStr[] = { 'a', 's', 'd', 'a', 's', 'd', 'c', 'c', 'c', '\0' };
	PCHAR pcPtr = szStr;
	printf("%s\n", szStr);
	g_TrimStingBothEnds(pcPtr, 'c');
	printf("%s\n", pcPtr);
	printf("2132");
	return 0;
}
