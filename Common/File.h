// File:   File.h
/*
 * �ļ������ļ���API
 * fopen        c�ṩAPI      ֧��windows linux
 * ifstream     c++�ṩAPI    ֧��windows linux
 * CreateFile   win32�ṩAPI  ֧��windows ֧��2G�����ļ�
 * open         linux�ṩAPI  ֧��linux
 * 
 * fopen 32λwindows linux ���ļ���С����2G ԭ�����α�����long(�з���32λ)
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
