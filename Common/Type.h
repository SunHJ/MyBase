#ifndef __PUBLIC_TYPE_H__
#define __PUBLIC_TYPE_H__
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
    typedef char                    CHAR;       // 1bytes
    typedef unsigned char           BYTE;       // 1bytes
    typedef unsigned short          WORD;       // 2bytes
    typedef unsigned int            DWORD;      // 4bytes
    typedef int 					LONG;       // 4bytes
    typedef signed   long long		INT64;      // 8bytes
    typedef unsigned long long		UINT64;     // 8bytes
	typedef unsigned int			ULONG_PTR;  // 4bytes
	typedef int						SOCKET;
#endif //PLATFORM_OS_WINDOWS

typedef unsigned int			UINT;
typedef float					FLOAT;
typedef double					DOUBLE;
typedef	std::string				STRING;

typedef       VOID *            PVOID;
typedef const VOID *            PCVOID;
typedef       VOID * const      CPVOID;
typedef const VOID * const      CPCVOID;

typedef       BYTE *            PBYTE;
typedef const BYTE *            PCBYTE;
typedef       BYTE * const      CPBYTE;
typedef const BYTE * const      CPCBYTE;

typedef       CHAR *            PCHAR;
typedef const CHAR *            PCCHAR;
typedef       CHAR * const      CPCHAR;
typedef const CHAR * const      CPCCHAR;

typedef       INT *             PINT;
typedef const INT *             PCINT;
typedef       INT * const       CPINT;
typedef const INT * const       CPCINT;

typedef       FLOAT *           PFLOAT;
typedef const FLOAT *           PCFLOAT;
typedef       FLOAT * const     CPFLOAT;
typedef const FLOAT * const     CPCFLOAT;

typedef       DOUBLE *          PDOUBLE;
typedef const DOUBLE *          PCDOUBLE;
typedef       DOUBLE * const    CPDOUBLE;
typedef const DOUBLE * const    CPCDOUBLE;

/*----------------------------------- GLOBAL CLASS DEFINE -----------------------------------*/
class UnCopyable
{
protected:
	UnCopyable()  {}
	~UnCopyable() {}

private:
	UnCopyable(CONST UnCopyable &);
	UnCopyable &operator=(CONST UnCopyable &);
};

class UnConstructable
{
private:
	UnConstructable() {}
	UnConstructable(CONST UnConstructable &);
	UnConstructable &operator=(CONST UnConstructable &);

protected:
	~UnConstructable() {}
};

template <class T>
class Singleton : private UnConstructable
{
public:
	static T& GetInstance() {
		static T l_sInstance;
		return l_sInstance;
	}
};

#define DECLARE_SINGLETON_PATTERN(T) friend class Singleton<T>
#define SINGLETON_GET_REF(T)         (Singleton<T>::GetInstance())
#define SINGLETON_GET_PTR(T)         (&Singleton<T>::GetInstance())

#endif //__PUBLIC_TYPE_H__
