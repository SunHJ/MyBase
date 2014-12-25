#ifndef __PUBLIC_GLOBAL_H__
#define __PUBLIC_GLOBAL_H__
#include "Macro.h"
#include "Type.h"


/*--------------------------------- GLOBAL VARIABLE DEFINE ----------------------------------*/

CONST int MAX_FILE_PATH_LEN = 260;                // Include '\0' character
CONST int MAX_FILE_NAME_LEN = 256;                // Include '\0' character
CONST int MAX_FILE_EXT_LEN	= 256;                // Include '\0' character
CONST int MAX_DIR_NAME_LEN	= 256;                // Include '\0' character

CONST int MAX_IP_ADDR_LEN = 16;                     // Include '\0' character
CONST DWORD INVALID_ID_VALUE = static_cast<DWORD>(-1); // Invalid number id of a string

/*---------------------------------- GLOBAL FUNCTION DEFINE ---------------------------------*/
template <class T>
inline T g_Max(CONST T &lhs,CONST T &rhs)
{
	return lhs > rhs ? lhs : rhs;
}

template <class T>
inline T g_Min(CONST T &lhs,CONST T &rhs)
{
	return lhs > rhs ? rhs : lhs;
}

template <class T>
inline VOID g_Swap(T &lhs, T &rhs)
{
	T temp = lhs;           // copy   content
	lhs = rhs, rhs = temp;  // switch content
}

template <class T>
inline BYTE g_GetMemBYTE(CONST T *CONST p)
{
	return *((BYTE*)(p));
}

template <class T>
inline WORD g_GetMemWORD(CONST T *CONST p)
{
	return *((WORD*)(p));
}

template <class T>
inline DWORD g_GetMemDWORD(CONST T *CONST p)
{
	return *((DWORD *)(p));
}

template <class T>
inline VOID g_SafelyIncrement(T &value)
{
	value = ((value)+1 > (value)) ? (value)+1 : (value);
}

template<class T>
inline VOID g_ZeroMemory(T *p, size_t nMemSize)
{
	CHECK_POINTER_RETURN_VOID_QUIET(p);
	::memset((VOID *)p, 0, nMemSize);
}

template <class T>
inline VOID g_SafelyDeletePtr(T *&p)
{
	// check if type is complete
	typedef CHAR TypeMustBeComplete[sizeof(T) ? 1 : -1];
	(VOID)sizeof(TypeMustBeComplete);

	if (NULL != p)
	{
		delete p; p = NULL;
	}
}

template <class T>
inline VOID g_SafelyDeleteArrayPtr(T *&p)
{
	// check if type is complete 
	typedef CHAR TypeMustBeComplete[sizeof(T) ? 1 : -1];
	(VOID)sizeof(TypeMustBeComplete);

	if (NULL != p)
	{
		delete[] p; p = NULL;
	}
}

template <class T>
inline VOID g_SafelyDeleteCONSTPtr(T *CONST p)
{
	// check if type is complete
	typedef CHAR TypeMustBeComplete[sizeof(T) ? 1 : -1];
	(VOID)sizeof(TypeMustBeComplete);

	if (NULL != p)
		delete p;
}

template <class T>
inline VOID g_SafelyDeleteCONSTArrayPtr(T *CONST p)
{
	// check if type is complete 
	typedef CHAR TypeMustBeComplete[sizeof(T) ? 1 : -1];
	(VOID)sizeof(TypeMustBeComplete);

	if (NULL != p)
		delete[] p;
}


inline PVOID g_AllocMemeoy(size_t nMemSize)
{
	CHECK_RETURN_CODE(nMemSize>0, NULL);

	PVOID pMem = ::malloc(nMemSize);
	CHECK_RETURN_CODE(NULL != pMem, NULL);

	g_ZeroMemory(pMem, nMemSize);
	return pMem;
}

template <class T>
inline VOID g_FreeMemeoy(T *&p)
{
	if (NULL != p)
	{
		::free(p); p = NULL;
	}
}

template <class T>
inline VOID g_FreeCONSTPtr(T *CONST p)
{
	if (NULL != p)
		::free(p);
}

template <class T>
inline VOID g_SetErrorCode(T *pErrorCode, T code)
{
	if (NULL != pErrorCode)
		*pErrorCode = code;
}

inline BOOL g_CheckHandle(CONST HANDLE &handle)
{
	CHECK_RETURN_BOOL_QUIET(NULL != handle && INVALID_HANDLE_VALUE != handle);
	return TRUE;
}

inline VOID g_CloseHandle(HANDLE &handle)
{
#ifdef WIN32
	if (NULL != handle && INVALID_HANDLE_VALUE != handle)
	{
		ASSERT(::CloseHandle(handle));
		handle = INVALID_HANDLE_VALUE;
	}
#endif
}

inline DWORD g_HashString2ID(CPCCHAR cpcString)
{
	CHECK_C_STRING_RETURN_CODE(cpcString, INVALID_ID_VALUE);

	DWORD dwID = 0;
	for (INT i = 0; '\0' != cpcString[i]; i++)
	{
		dwID = (dwID + (i + 1) * cpcString[i]) % 0x8000000C * 0xFFFFFFEF;
	}

	return (dwID ^ 0x12345678);
}

inline VOID g_ConvertToHexString(
	CPCCHAR         cpcSrc,
	CONST size_t    nSrcLen,
	PCHAR			pDst,
	CONST size_t    nDstLen)
{
	CHECK_RETURN_VOID(NULL != cpcSrc && nSrcLen>0);
	CHECK_RETURN_VOID(NULL != pDst && nDstLen>nSrcLen * 2);
	g_ZeroMemory(pDst, nDstLen);

	CHAR pTemp[3];
	for (size_t i = 0; i<nSrcLen; i++)
	{
		g_ZeroMemory(pTemp, 3);
		::sprintf(pTemp, "%02X", (BYTE)cpcSrc[i]);
		::strcat(pDst, pTemp);
	}

	pDst[nDstLen - 1] = '\0';
}

inline BOOL g_IsBlankCharacter(CONST CHAR c)
{
	if (c == ' ' || c == '\t')
		return TRUE;
	return FALSE;
}

inline BOOL g_IsEnterCharacter(CONST CHAR c)
{
	if (c == '\r' || c == '\n')
		return TRUE;
	return FALSE;
}

inline STRING g_TrimString(CONST STRING &s,CONST CHAR c)
{
	STRING temp = s;
	if (temp.empty())  
		return temp;

	STRING::size_type i = 0, j = 0, len = temp.size();
	while (i < len)
	{
		j = temp.find_first_of(c, i);
		if (j == STRING::npos) 
			break;
		temp.erase(j, 1);
		i = j;
	}

	return temp;
}

inline VOID g_TrimString(CONST PCHAR pString, CONST CHAR c)
{
	CHECK_C_STRING_RETURN_VOID(pString);

	size_t nStringLen = ::strlen(pString);
	CHAR * pTemp = (CHAR *)::malloc(nStringLen);
	g_ZeroMemory(pTemp, nStringLen);

	for (size_t i = 0; i < nStringLen; i++)
	{
		if (pString[i] != c)
			pTemp[i] = pString[i];
	}

	::memcpy(pString, pTemp, nStringLen);
	g_FreeMemeoy(pTemp);
}

inline VOID g_TrimStingBothEnds(CONST PCHAR pString, CONST CHAR c)
{
	CHECK_C_STRING_RETURN_VOID(pString);

	size_t nStringLen = ::strlen(pString);
	size_t nHead = 0, nTail = nStringLen - 1;

	for (size_t i = 0; i < nStringLen; i++)
	{
		if (pString[nHead] == c)
			nHead++;

		if (pString[nTail] == c)
			nTail--;

		if (nHead > nTail)
			break;
	}

	if (nHead>nTail || nHead == nStringLen || nTail<0)
	{
		g_ZeroMemory(pString, nStringLen);
		return;
	}
	::memmove(pString, pString + nHead, nTail - nHead + 1);
	pString[nTail - nHead + 1] = '\0';
}

inline VOID g_TrimStringBlankChar(CONST PCHAR pString)
{
	CHECK_C_STRING_RETURN_VOID(pString);

	size_t nStringLen = ::strlen(pString);
	CHAR * pTemp = (CHAR *)g_AllocMemeoy(nStringLen);
	g_ZeroMemory(pTemp, nStringLen);

	size_t j = 0;
	for (size_t i = 0; i<nStringLen; i++)
	{
		if (!g_IsBlankCharacter(pString[i]))
			pTemp[j++] = pString[i];
	}
	::memcpy(pString, pTemp, nStringLen);
	g_FreeMemeoy(pTemp);
}

inline VOID g_TrimStringBlankCharBothEnds(CONST PCHAR pString)
{
	CHECK_C_STRING_RETURN_VOID_QUIET(pString);

	size_t nStringLen = ::strlen(pString);
	size_t nHead = 0, nTail = nStringLen - 1;

	for (size_t i = 0; i<nStringLen; i++)
	{
		if (g_IsBlankCharacter(pString[nHead]) || g_IsEnterCharacter(pString[nHead]))
			nHead++;
		if (g_IsBlankCharacter(pString[nTail]) || g_IsEnterCharacter(pString[nTail]))
			nTail--;
		if (nHead > nTail)
			break;
	}

	if (nHead>nTail || nHead == nStringLen || nTail<0)
	{
		g_ZeroMemory(pString, nStringLen);
		return;
	}
	nStringLen = nTail - nHead + 1;
	::memmove(pString, pString + nHead, nStringLen);
	pString[nStringLen] = '\0';
}

inline VOID g_PadWithBlanks(CONST PCHAR pBuffer, CONST size_t nBufferLen)
{
	CHECK_RETURN_VOID(NULL != pBuffer && nBufferLen >= 0);

	size_t i = 0;
	for (; i<nBufferLen; i++)
	{
		if (pBuffer[i] == '\0')
			break;
	}

	for (; i<nBufferLen - 1; i++)
	{
		pBuffer[i] = ' ';
	}

	pBuffer[nBufferLen - 1] = '\0';
}

#ifdef WIN32

inline LONG g_InterlockedIncrement(LONG *plValue)
{
#ifdef _MT
	return ::InterlockedIncrement(plValue);
#else
	return ++value;
#endif
}

inline LONG g_InterlockedDecrement(LONG *plValue)
{
#ifdef _MT
	return ::InterlockedDecrement(plValue);
#else
	return --value;
#endif
}

inline LONG g_InterlockedExchange(volatile LONG *pTarget, LONG lValue)
{
#ifdef _MT
	return ::InterlockedExchange(pTarget, lValue);
#else
	LONG lTemp = *pTarget;
	*pTarget = lValue;
	return lTemp;
#endif
}

inline LONG g_InterlockedExchangeAdd(LONG *plValue, LONG lAddValue)
{
#ifdef _MT
	return ::InterlockedExchangeAdd(plValue, lAddValue);
#else
	LONG lResult = *plValue;
	*plValue += lAddValue;
	return lResult;
#endif
}

#else

inline LONG g_InterlockedIncrement(LONG *plValue)
{
	LONG lResult;
	__asm__ __volatile__
	(
		"movl $0x01, %%eax\n"                                           // 0x01 -> eax
		"lock xaddl %%eax, (%1)\n"                                      // plValue和eax内容交换 plValue += eax
		"incl %%eax\n"                                                  // eax += 1
		:
		"=a"(lResult)
		:
		"c"(plValue)
		:
		"memory"
	);
	return lResult;
}

inline LONG g_InterlockedDecrement(LONG *plValue)
{
	LONG lResult;
	__asm__ __volatile__
	(
		"movl $0xffffffff, %%eax\n"
		"lock xaddl %%eax, (%1)\n"
		"decl %%eax\n"
		:
		"=a"(lResult)
		:
		"c"(plValue)
		:
		"memory"
	);
	return lResult;
}

inline LONG g_InterlockedExchange(volatile LONG *pTarget, LONG lValue)
{
	__asm__ __volatile__
	(
		"xchgl %k0, %1"                                                 // xchgl执行时默认会锁内存总线，%k0 : 32位的寄存器,
		:
		"=r" (lValue)                                               // %0 : 任一通用寄存器返回
		:
		"m" (*((volatile LONG *)pTarget)), "0" (lValue)              // %1 - m表示操作数存放在内存， "0″表示指定第0个输出变量(lValue)相同的约束
		:
		"memory"
	);
	return lValue;
}

inline LONG g_InterlockedExchangeAdd(LONG *plValue, LONG lAddValue)
{
	LONG lResult;
	__asm__ __volatile__
	(
		"lock xaddl %%eax, (%1)\n"
		:
		"=a"(lResult)
		:
		"c"(plValue), "a"(lAddValue)
		:
		"memory"
	);
	return lResult;
}

#endif  // WIN32

#endif	//__PUBLIC_GLOBAL_H__
