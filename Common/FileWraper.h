#ifndef __COMMON_FILE_WRAPPER_H__
#define __COMMON_FILE_WRAPPER_H__

#include "Macro.h"
#include "Type.h"

#ifdef PLATFORM_OS_WINDOWS
	#define FILE_LINE_BREAK "\r\n"
#else
	#define FILE_LINE_BREAK "\n"
#endif

inline BOOL g_CheckFile(CONST FILE *hFile)
{
	CHECK_RETURN_BOOL(NULL != hFile);
	return TRUE;
}

inline BOOL g_CloseFile(FILE *&hFile)
{
	if (NULL != hFile)
	{
		INT nRetCode = ::fclose(hFile);
		if (EOF == nRetCode)
			return FALSE;
		hFile = NULL;
	}
	return TRUE;
}

inline BOOL g_OpenFile(FILE* &hFile, CPCCHAR cpcFile, CPCCHAR cpcMode)
{
	CHECK_C_STRING_RETURN_BOOL(cpcFile);
	CHECK_C_STRING_RETURN_BOOL(cpcMode);

	g_CloseFile(hFile);
	hFile = ::fopen(cpcFile, cpcMode);
	CHECK_POINTER_RETURN_BOOL(NULL != hFile);

	return TRUE;
}

inline BOOL g_SeekFile(FILE* &hFile, LONG lOffset, INT nFromWhere)
{
	BOOL bReCode = FALSE;
	bReCode = g_CheckFile(hFile);
	CHECK_RETURN_BOOL(bReCode);

	bReCode = ::fseek(hFile, lOffset, nFromWhere);
	CHECK_RETURN_BOOL(bReCode == 0);
	return TRUE;
}

inline BOOL g_SeekFileSet(FILE* &hFile, LONG lOffset)
{
	CHECK_RETURN_BOOL(lOffset >= 0);
	return g_SeekFile(hFile, lOffset, SEEK_SET);
}

inline BOOL g_SeekFileCur(FILE* &hFile, LONG lOffset)
{
	return g_SeekFile(hFile, lOffset, SEEK_CUR);
}

inline BOOL g_SeekFileEnd(FILE* &hFile, LONG lOffset)
{
	CHECK_RETURN_BOOL(lOffset <= 0);
	return g_SeekFile(hFile, lOffset, SEEK_END);
}

inline LONG g_TellFile(FILE* &hFile)
{
	LONG lResult = 0;
	BOOL bReCode = FALSE;
	bReCode = g_CheckFile(hFile);
	CHECK_RETURN_CODE(bReCode, -1);

	lResult = ::ftell(hFile);
	return lResult;
}

inline LONG g_ReadFile(FILE* &hFile, CONST CPVOID cpBuffer, size_t lSize)
{
	CHECK_RETURN_CODE(hFile, -1);
	CHECK_RETURN_CODE(NULL != cpBuffer, -1);
	CHECK_RETURN_CODE(lSize > 0, -1);

	size_t nReCode = 0;
	nReCode = ::fread(cpBuffer, lSize, 1, hFile);
	CHECK_RETURN_CODE(nReCode != -1, -1);
	CHECK_RETURN_CODE(::ferror(hFile) == 0, -1);

	return static_cast<LONG>(nReCode);
}

inline LONG g_WriteFile(FILE* &hFile, CPCVOID cpcBuffer, size_t lSize)
{
	CHECK_RETURN_CODE(hFile, -1);
	CHECK_RETURN_CODE(NULL != cpcBuffer, -1);
	CHECK_RETURN_CODE(lSize > 0, -1);
	
	size_t nReCode = 0;
	nReCode = ::fwrite(cpcBuffer, lSize, 1, hFile);
	CHECK_RETURN_CODE(nReCode != -1, -1);
	CHECK_RETURN_CODE(::ferror(hFile) == 0, -1);

	return static_cast<LONG>(nReCode);
}

inline LONG g_ReadString(FILE* &hFile, CONST CPCHAR cpString, LONG lReadLen)
{
	CHECK_RETURN_CODE(hFile, -1);
	CHECK_RETURN_CODE(NULL != cpString, -1);

	PCHAR pStr = NULL;
	pStr = ::fgets(cpString, lReadLen, hFile);
	CHECK_RETURN_CODE(NULL != pStr, -1);
	CHECK_RETURN_CODE(::ferror(hFile) == 0, -1);

	size_t nResult = ::strlen(pStr);
	return static_cast<LONG>(nResult);
}

inline LONG g_WriteString(FILE* &hFile, CPCCHAR cpcString)
{
	CHECK_RETURN_CODE(hFile, -1);
	CHECK_RETURN_CODE(NULL != cpcString, -1);

	INT nReCode = 0;
	nReCode = ::fputs(cpcString, hFile);
	CHECK_RETURN_CODE(nReCode != EOF, -1);

	CHECK_RETURN_CODE(::ferror(hFile) == 0, -1);

	return static_cast<LONG>(strlen(cpcString));
}

inline BOOL g_IsDirExisted(CPCCHAR cpcDirPath)
{
	CHECK_C_STRING_RETURN_BOOL(cpcDirPath);
#ifdef PLATFORM_OS_WINDOWS
	DWORD dwRetCode = ::GetFileAttributes(cpcDirPath);
	if (FILE_ATTRIBUTE_DIRECTORY == dwRetCode)
	{
		return TRUE;
	}
#else
	struct stat buf;
	if (0 == ::stat(cpcDirPath, &buf))
	{
		return (S_ISDIR(buf.st_mode) != 0);
	}
#endif
	return FALSE;
}

inline BOOL g_IsFileExisted(CPCCHAR cpcFilePath)
{
	CHECK_C_STRING_RETURN_BOOL(cpcFilePath);
#ifdef PLATFORM_OS_WINDOWS
	DWORD dwRetCode = ::GetFileAttributes(cpcFilePath);
	if (FILE_ATTRIBUTE_NORMAL == dwRetCode)
	{
		return TRUE;
	}
#else
	struct stat buf;
	if (0 == ::stat(cpcFilePath, &buf))
	{
		return (S_ISREG(buf.st_mode) != 0);
	}
#endif
	return FALSE;
}

inline BOOL g_FormatFilePath(STRING &strFilePath)
{
	ASSERT(strFilePath.size() >0 && strFilePath[0] != '\0');
	STRING::size_type i = strFilePath.find_first_not_of("\t ");
	STRING::size_type j = strFilePath.find_last_not_of("\t \\/");
	if (STRING::npos != i && STRING::npos != j && i<= j)
		strFilePath = strFilePath.substr(i, j-i+1);
	else
		strFilePath = "";
	return TRUE;
}

inline BOOL g_FormatDirPath(STRING &strDirPath)
{
	g_FormatFilePath(strDirPath);

	if (strDirPath != "")
		strDirPath += "/";
	return TRUE;
}

inline BOOL g_DeleteFile(CONST STRING &strDirPath)
{
	CHECK_RETURN_BOOL(strDirPath.size()>0 && strDirPath[0]!='\0');
	INT nReCode = ::remove(strDirPath.c_str());
	if (nReCode == 0)
	{
		return TRUE;
	}
	return FALSE;
}

inline BOOL g_DeleteDir(CONST STRING &strDirPath)
{
	CHECK_RETURN_BOOL(strDirPath.size()>0 && strDirPath[0] != '\0');

	STRING strBasePath = strDirPath;
	g_FormatDirPath(strBasePath);

	if (!g_IsDirExisted(strBasePath.c_str()))
		return TRUE;
	STRING strFindPath = strBasePath + "*";

	WIN32_FIND_DATA FindData;
	HANDLE hFindFile = ::FindFirstFile(strFindPath.c_str(), &FindData);
	CHECK_RETURN_BOOL(INVALID_HANDLE_VALUE != hFindFile);

	BOOL bIsFind = TRUE;
	while (bIsFind)
	{
		if (::strcmp(FindData.cFileName, ".")!=0 && ::strcmp(FindData.cFileName, "..")!=0)
		{
			STRING strTempPath = strBasePath + FindData.cFileName;
			if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				g_DeleteDir(strTempPath.c_str());
			else
			{
				::SetFileAttributes(strTempPath.c_str(), FILE_ATTRIBUTE_NORMAL);
				g_DeleteFile(strTempPath.c_str());
			}
		}
		bIsFind = ::FindNextFile(hFindFile, &FindData);
	}

	BOOL bRetCode = ::RemoveDirectory(strBasePath.c_str());
	::FindClose(hFindFile);

	return bRetCode;
}

inline BOOL g_CreateDir(CONST STRING &strDirPath)
{
	CHECK_RETURN_BOOL(strDirPath.size()>0 && strDirPath[0]!='\0');

	STRING strRealFilePath = strDirPath;
	g_FormatDirPath(strRealFilePath);

	STRING::size_type i    = strRealFilePath.find_first_of("/\\");
	STRING strCheckingPath = strRealFilePath.substr(0, i + 1);
	STRING strToCheckPath  = strRealFilePath.substr(i + 1, strRealFilePath.size() - i);

	BOOL bLoopFlag = TRUE;
	while(bLoopFlag) {
		if (!g_IsDirExisted(strCheckingPath.c_str()))
			CHECK_RETURN_BOOL(::CreateDirectory(strCheckingPath.c_str(), NULL));

		if (!strToCheckPath.size()) break;

		STRING::size_type j = strToCheckPath.find_first_of("/\\");
		strCheckingPath += strToCheckPath.substr(0, j + 1);
		strToCheckPath = strToCheckPath.substr(j + 1, strToCheckPath.size() - j);
	}

	return TRUE;
}

#endif //__COMMON_FILE_WRAPPER_H__