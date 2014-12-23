#ifndef __MACRO_H__
#define __MACRO_H__

#include "Platform.h"

#ifdef __cplusplus              // C++

	#ifndef NULL
	#define NULL        0
	#endif // NULL

#else                           // C
	#ifndef NULL
	#define NULL        ((void*)0)
	#endif // NULL

#endif // __cplusplus

/*------------------------------------------------------------------------------------------
"__pragma(warning(push))"                 means  "#pragma warning(push)"
"__pragma(warning(disable:warning_code))" means  "#pragma warning(disable:warning_code)"
"__pragma(warning(pop))"                  means  "#pragma warning(pop)"

MS VC++ 11.0    Visual C++ 2012     _MSC_VER = 1700
MS VC++ 10.0    Visual C++ 2010     _MSC_VER = 1600
MS VC++ 9.0     Visual C++ 2008     _MSC_VER = 1500
MS VC++ 8.0     Visual C++ 2005     _MSC_VER = 1400
MS VC++ 7.1     Visual C++ 2003     _MSC_VER = 1310
MS VC++ 7.0                         _MSC_VER = 1300
MS VC++ 6.0                         _MSC_VER = 1200
MS VC++ 5.0                         _MSC_VER = 1100
------------------------------------------------------------------------------------------*/
#undef DISABLE_WARNING
#ifdef _MSC_VER                 // MS VC++
#define DISABLE_WARNING(warning_code, expression) \
	__pragma(warning(push)) \
	__pragma(warning(disable:warning_code)) expression \
	__pragma(warning(pop))
#else                           // GCC
#define DISABLE_WARNING(warning_code, expression) \
	expression
#endif // _MSC_VER

#undef  WHILE_FALSE_NO_WARNING
#define WHILE_FALSE_NO_WARNING DISABLE_WARNING(4127, while(FALSE))

#undef  ASSERT_FALSE
#define ASSERT_FALSE(condition)  assert(FALSE && #condition)

#define ASSERT(condition) assert(condition)

// ========================================== ASSERT MACROS ========================================== 
// Macro : PROCESS_ERROR
#undef  PROCESS_ERROR
#define PROCESS_ERROR(condition) \
	do { \
		if (!(condition)) { \
		ASSERT_FALSE(condition); \
		goto Exit0; \
		} \
	} WHILE_FALSE_NO_WARNING

// Macro : CHECK_RETURN_CODE
#undef  CHECK_RETURN_CODE
#define CHECK_RETURN_CODE(condition, ret_code) \
	do { \
		if (!(condition)) { \
			ASSERT_FALSE(condition); \
			return ret_code; \
		} \
	} WHILE_FALSE_NO_WARNING

// Macro : CHECK_RETURN_VOID
#undef  CHECK_RETURN_VOID
#define CHECK_RETURN_VOID(condition) \
	do { \
		if (!(condition)) { \
			ASSERT_FALSE(condition); \
			return; \
		} \
	} WHILE_FALSE_NO_WARNING

// Macro : CHECK_RETURN_BOOL
#undef  CHECK_RETURN_BOOL
#define CHECK_RETURN_BOOL(condition)	CHECK_RETURN_CODE(condition, FALSE)

// Macro : CHECK_C_STRING_RETURN_BOOL
#undef  CHECK_C_STRING_RETURN_BOOL
#define CHECK_C_STRING_RETURN_BOOL(s)	CHECK_RETURN_BOOL(NULL != s && '\0' != s[0])

// Macro : CHECK_C_STRING_RETURN_VOID
#undef  CHECK_C_STRING_RETURN_VOID
#define CHECK_C_STRING_RETURN_VOID(s)	CHECK_RETURN_VOID(NULL != s && '\0' != s[0])

// Macro : CHECK_C_STRING_RETURN_CODE
#undef  CHECK_C_STRING_RETURN_CODE
#define CHECK_C_STRING_RETURN_CODE(s, ret_code)	 CHECK_RETURN_CODE((NULL != s && '\0' != s[0]), ret_code)

// Macro : CHECK_POINTER_RETURN_BOOL
#undef  CHECK_POINTER_RETURN_BOOL
#define CHECK_POINTER_RETURN_BOOL(p)	CHECK_RETURN_BOOL(NULL != p)

// Macro : CHECK_POINTER_RETURN_VOID
#undef  CHECK_POINTER_RETURN_VOID
#define CHECK_POINTER_RETURN_VOID(p)	CHECK_RETURN_VOID(NULL != p)

// Macro : CHECK_POINTER_RETURN_CODE
#undef  CHECK_POINTER_RETURN_CODE
#define CHECK_POINTER_RETURN_CODE(p, ret_code)  CHECK_RETURN_CODE(NULL != p, ret_code)

// ========================================== QUIET MACROS ========================================== 
// Macro : PROCESS_ERROR_QUIET
#undef  PROCESS_ERROR_QUIET
#define PROCESS_ERROR_QUIET(condition) \
	do { \
		if (!(condition)) { \
		goto Exit0; \
		} \
	} WHILE_FALSE_NO_WARNING

// Macro : CHECK_RETURN_CODE_QUIET
#undef  CHECK_RETURN_CODE_QUIET
#define CHECK_RETURN_CODE_QUIET(condition, ret_code) \
	do { \
		if (!(condition)) { \
			return ret_code; \
		} \
	} WHILE_FALSE_NO_WARNING


// Macro : CHECK_RETURN_BOOL_QUIET
#undef  CHECK_RETURN_BOOL_QUIET
#define CHECK_RETURN_BOOL_QUIET(condition) CHECK_RETURN_CODE_QUIET(condition, FALSE)

// Macro : CHECK_RETURN_VOID_QUIET
#undef  CHECK_RETURN_VOID_QUIET
#define CHECK_RETURN_VOID_QUIET(condition) \
	do { \
		if (!(condition)) { \
			return; \
		} \
	} WHILE_FALSE_NO_WARNING

// Macro : CHECK_C_STRING_RETURN_BOOL_QUIET
#undef  CHECK_C_STRING_RETURN_BOOL_QUIET
#define CHECK_C_STRING_RETURN_BOOL_QUIET(s) CHECK_RETURN_BOOL_QUIET(NULL != s && '\0' != s[0])

// Macro : CHECK_C_STRING_RETURN_VOID_QUIET
#undef  CHECK_C_STRING_RETURN_VOID_QUIET
#define CHECK_C_STRING_RETURN_VOID_QUIET(s) CHECK_RETURN_VOID_QUIET(NULL != s && '\0' != s[0])

// Macro : CHECK_C_STRING_RETURN_CODE_QUIET
#undef  CHECK_C_STRING_RETURN_CODE_QUIET
#define CHECK_C_STRING_RETURN_CODE_QUIET(s, ret_code) CHECK_RETURN_CODE_QUIET((NULL != s && '\0' != s[0]), ret_code)

// Macro : CHECK_POINTER_RETURN_BOOL_QUIET
#undef  CHECK_POINTER_RETURN_BOOL_QUIET
#define CHECK_POINTER_RETURN_BOOL_QUIET(p) CHECK_RETURN_BOOL_QUIET(NULL != p)

// Macro : CHECK_POINTER_RETURN_VOID_QUIET
#undef  CHECK_POINTER_RETURN_VOID_QUIET
#define CHECK_POINTER_RETURN_VOID_QUIET(p) CHECK_RETURN_VOID_QUIET(NULL != p)

// Macro : CHECK_POINTER_RETURN_CODE_QUIET
#undef  CHECK_POINTER_RETURN_CODE_QUIET
#define CHECK_POINTER_RETURN_CODE_QUIET(p, ret_code) CHECK_RETURN_CODE_QUIET(NULL != p, ret_code)

// Macro       : FETCH_FIELD_SIZE
// Description : fetch the size of field in a struct or class.
// Param       : type : class or struct name
//               field : member variable name
#define FETCH_FIELD_SIZE(type, field) sizeof(((type *)0)->field)

// Macro       : FETCH_FIELD_OFFSET
// Description : fetch the offset of field in a struct or class.
// Param       : type : class or struct name
//               field : member variable name
#define FETCH_FIELD_OFFSET(type, field) ((ULONG_PTR)&((type *)0)->field)

// Macro       : FETCH_ADDRESS_BY_FIELD
// Description : fetch the size of class or struct by member variable address
// Param       : address : the address of member variable instance
//               type    : class or struct name
//               field   : member variable name
#ifdef PLATFORM_OS_WINDOWS      // PLATFORM_OS_WINDOWS
	#define FETCH_ADDRESS_BY_FIELD(address, type, field) CONTAINING_RECORD(address, type, field)
#else                           // PLATFORM_OS_LINUX
//	#define FETCH_ADDRESS_BY_FIELD(address, type, field) ((type *)((char *)(address) - (ULONG_PTR)(&((type *)0)->field)))
	#define FETCH_ADDRESS_BY_FIELD(address, type, field) ((type *)((char *)(address)-BZ_FETCH_FIELD_OFFSET(type, field)))
#endif // PLATFORM_OS_WINDOWS

#define DEBUG_PRINT_INT(expression)                          \
	printf("[DEBUG PRINT] Expression \"%s\" returned: %d\n", \
	(#expression), (expression))

#define DEBUG_PRINT_BOOL(expression)                         \
	printf("[DEBUG PRINT] Expression \"%s\" returned: %s\n", \
	(#expression), (expression) == TRUE ? "TRUE" : "FALSE")

#pragma warning(disable:4996) 

#endif // __MACRO_H__