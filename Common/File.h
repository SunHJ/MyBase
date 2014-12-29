// File:   File.h
/*
 * 文件操作的几类API
 * fopen        c提供API      支持windows linux
 * ifstream     c++提供API    支持windows linux
 * CreateFile   win32提供API  支持windows 支持2G以上文件
 * open         linux提供API  支持linux
 * 
 * fopen 32位windows linux 打开文件大小限制2G 原因是游标类型long(有符号32位)
 * fopen64
 */
#ifndef __COMMON_FILE_H__
#define __COMMON_FILE_H__

#include "Type.h"

// < 2G
class File
{
public:
	File();
	~File();

	virtual BOOL Open(CPCCHAR cpcFile, CPCCHAR cpcMode);
    virtual BOOL Close();

    virtual BOOL SeekBegin(LONG lOffset);
    virtual BOOL SeekEnd(LONG lOffset);
    virtual BOOL SeekCurrent(LONG lOffset);

    virtual LONG Size() CONST;
    virtual LONG Tell() CONST;
    virtual BOOL Flush();

    virtual BOOL IsEof() CONST;
    virtual BOOL IsOpen() CONST;

    virtual LONG Read (CONST PCHAR cpBuffer, LONG lBufferSize, LONG lReadSize);
    virtual LONG Write(CPCCHAR cpcBuffer, LONG lBufferSize, LONG lWriteSize);

    virtual LONG ReadTextLine(CONST PCHAR cpStr, LONG lReadLen);
    virtual LONG WriteTextLine(CPCCHAR cpcBuffer, LONG lBufferSize);

private:
    FILE *m_hFile;
    BOOL  m_bBinaryFile;
};

// >= 4G
class BLargeFile/* : public BIFile*/
{

};

#endif // __COMMON_FILE_H__
