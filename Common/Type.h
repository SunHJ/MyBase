#ifndef __TYPE_H__
#define __TYPE_H__
#include "Platform.h"

/*------------------------------------------------------------------------------------------
<Type>       <Windows-32bits>     <Windows-64bits>     <Linux-32bits>     <Linux-64bits>
char                1                   1                   1                   1
short               2                   2                   2                   2
int                 4                   4                   4                   4
long                4                   4                   4                   8
long long           8                   8                   8                   8
float               4                   4                   4                   4
double              8                   8                   8                   8
long double         12                  16                  12                  16
pointer             4                   8                   4                   8
size_t              4                   8                   4                   8
------------------------------------------------------------------------------------------*/

#ifdef PLATFORM_OS_WINDOWS
#else
typedef int                     BOOL;       // 4bytes
typedef int						INT;		// 4bytes
typedef unsigned char           BYTE;       // 1bytes
typedef char					CHAR;		// 1bytes
typedef unsigned short          WORD;       // 2bytes
typedef unsigned int            DWORD;      // 4bytes
typedef long					LONG;       // 4bytes
typedef signed   long long		INT64;      // 8bytes
typedef unsigned long long		UINT64;     // 8bytes
typedef unsigned int			ULONG_PTR;  // 4bytes
#endif //PLATFORM_OS_WINDOWS

typedef float					FLOAT;
typedef double					DOUBLE;
typedef	std::string				STRING;
typedef const void   *          PCVOID;
typedef       void   * const    CPVOID;
typedef const void   * const    CPCVOID;

typedef const BYTE   *          PCBYTE;
typedef       BYTE   * const    CPBYTE;
typedef const BYTE   * const    CPCBYTE;

typedef const CHAR   *          PCCHAR;
typedef       CHAR   * const    CPCHAR;
typedef const CHAR   * const    CPCCHAR;

typedef const INT    *          PCINT;
typedef       INT    * const    CPINT;
typedef const INT    * const    CPCINT;

typedef const FLOAT  *          PCFLOAT;
typedef       FLOAT  * const    CPFLOAT;
typedef const FLOAT  * const    CPCFLOAT;

typedef const DOUBLE *          PCDOUBLE;
typedef       DOUBLE * const    CPDOUBLE;
typedef const DOUBLE * const    CPCDOUBLE;


#endif //__TYPE_H__
