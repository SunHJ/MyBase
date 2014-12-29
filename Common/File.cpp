#include "File.h"
#include "Macro.h"
#include "FileWraper.h"

File::File() : m_hFile(NULL), m_bBinaryFile(FALSE)
{
}

File::~File()
{
	Close();
}

BOOL File::Open(CPCCHAR cpcFile, CPCCHAR cpcMode)
{
    CHECK_C_STRING_RETURN_BOOL(cpcFile);
    CHECK_C_STRING_RETURN_BOOL(cpcMode);

	INT nReCode = 0;
    CHECK_RETURN_BOOL(m_hFile == NULL);

	nReCode = g_OpenFile(m_hFile, cpcFile, cpcMode);
	CHECK_RETURN_BOOL(nReCode);

    if (::strchr(cpcMode, 'b'))
        m_bBinaryFile = TRUE;
    else
        m_bBinaryFile = FALSE;

    return TRUE;
}

BOOL File::Close()
{
    int nRetCode = 0;
    CHECK_RETURN_BOOL(m_hFile);

	nRetCode = g_CloseFile(m_hFile);
    CHECK_RETURN_BOOL(nRetCode);

    return TRUE;
}

LONG File::Size() CONST
{
	INT nRetCode = 0;
	LONG lResult = -1;
	LONG lOriginPos = -1;
	FILE* hFile = m_hFile;

	lOriginPos = g_TellFile(hFile);
    CHECK_RETURN_CODE(lOriginPos != -1, -1);

	nRetCode = g_SeekFileEnd(hFile, 0);
    CHECK_RETURN_CODE(nRetCode, -1);

	lResult = g_TellFile(hFile);
    CHECK_RETURN_CODE(lResult != -1, -1);

	nRetCode = g_SeekFileSet(hFile, lOriginPos);
    CHECK_RETURN_CODE(nRetCode, -1);

    return lResult;
}

BOOL File::SeekBegin(LONG lOffset)
{
    int nRetCode = 0;

    CHECK_RETURN_BOOL(lOffset <= Size());

    nRetCode = g_SeekFileSet(m_hFile, lOffset);
    CHECK_RETURN_BOOL(nRetCode);

    return TRUE;
}

BOOL File::SeekEnd(LONG lOffset)
{
    int nRetCode = 0;

    CHECK_RETURN_BOOL(lOffset >= 0);

	nRetCode = g_SeekFileEnd(m_hFile, -lOffset);
    CHECK_RETURN_BOOL(nRetCode);

    return TRUE;
}

BOOL File::SeekCurrent(LONG lOffset)
{
    int nRetCode = 0;

    CHECK_RETURN_BOOL(m_hFile);

    LONG lSize = Size();
    CHECK_RETURN_BOOL(lSize != -1);

    LONG lPos = Tell();
    CHECK_RETURN_BOOL(lPos != -1);

	LONG lValue = 0;
	if (lOffset > 0)
	{
		lValue = lSize - lPos;
		CHECK_RETURN_BOOL(lOffset < lValue);
	}
	else
	{
		lValue = -lOffset;
		CHECK_RETURN_BOOL(lValue < lPos);
	}
	
	return g_SeekFileCur(m_hFile, lOffset);

}

LONG File::Tell() CONST
{
	FILE* hFile = m_hFile;
	LONG lResult = g_TellFile(hFile);
    return lResult;
}

BOOL File::IsOpen() CONST
{
	return m_hFile != NULL;
}

BOOL File::Flush()
{
	CHECK_RETURN_BOOL(m_hFile);
	int nRetCode = ::fflush(m_hFile);
	CHECK_RETURN_BOOL(nRetCode == 0);

	return TRUE;
}

BOOL File::IsEof() const
{
	CHECK_RETURN_BOOL(m_hFile);

	return ::feof(m_hFile) != 0;
}

LONG File::Read(CONST PCHAR cpBuffer, LONG lBufferSize, LONG lReadSize)
{
    LONG lResult = -1;

    CHECK_RETURN_CODE(m_hFile, -1);
    CHECK_RETURN_CODE(lBufferSize > 0, -1);
    CHECK_RETURN_CODE(lReadSize > 0, -1);
    CHECK_RETURN_CODE(lReadSize <= lBufferSize, -1);

	lResult = g_ReadFile(m_hFile, cpBuffer, lReadSize);
    return lResult;
}

LONG File::Write(CPCCHAR cpcBuffer, LONG lBufferSize, LONG lWriteSize)
{
    LONG lResult = -1;

    CHECK_RETURN_CODE(m_hFile, -1);
    CHECK_RETURN_CODE(lBufferSize > 0, -1);
    CHECK_RETURN_CODE(lWriteSize > 0, -1);
    CHECK_RETURN_CODE(lWriteSize <= lBufferSize, -1);

	lResult = g_WriteFile(m_hFile, cpcBuffer, lWriteSize);
    return lResult;
}

LONG File::ReadTextLine(CONST PCHAR cpStr, LONG lReadLen)
{
    LONG lResult = -1;

    CHECK_RETURN_CODE(m_hFile, -1);
    CHECK_RETURN_CODE(!m_bBinaryFile, -1);
    CHECK_RETURN_CODE(cpStr, -1);
    CHECK_RETURN_CODE_QUIET(lReadLen != 0, 0);

    CHECK_RETURN_CODE(lReadLen > 0, -1);
	lResult = g_ReadString(m_hFile, cpStr, lReadLen);

	// 去除读取的行尾的\n
    char *pNewLine = &cpStr[lResult - 1];
    if (*pNewLine == '\n')
    {
        *pNewLine = '\0';
        --lResult;
    }

    return lResult;
}

LONG File::WriteTextLine(CPCCHAR cpcStr, LONG lWriteLen)
{
	LONG lResult = 0;
	LONG lReCode = 0;
    CHECK_RETURN_CODE(m_hFile, -1);
    CHECK_RETURN_CODE(!m_bBinaryFile, -1);
    CHECK_RETURN_CODE(cpcStr, -1);
    CHECK_RETURN_CODE_QUIET(cpcStr[0], 0);
    CHECK_RETURN_CODE_QUIET(lWriteLen != 0, 0);

    CHECK_RETURN_CODE(lWriteLen > 0, -1);

    LONG lStrLen = static_cast<LONG>(::strnlen(cpcStr, lWriteLen));
    CHECK_RETURN_CODE(lStrLen <= lWriteLen, -1);

	lReCode = g_WriteString(m_hFile, cpcStr);
	CHECK_RETURN_CODE(lReCode > 0, -1);
	lResult += lReCode;

	lReCode = g_WriteString(m_hFile, FILE_LINE_BREAK);
	CHECK_RETURN_CODE(lReCode > 0, -1);
	lResult += lReCode;

    return lResult;
}
