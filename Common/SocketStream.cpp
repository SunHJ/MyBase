#include "SocketStream.h"

AsyncSocketStream::AsyncSocketStream()
{
	m_hRemoteSocket				  = INVALID_SOCKET;
	m_strRemoteIp				  = "";
	m_usRemotePort				  = 0;
}

AsyncSocketStream::~AsyncSocketStream()
{
	g_CloseSocket(m_hRemoteSocket);
}

BOOL AsyncSocketStream::Init(SOCKET hRemoteSocket, STRING strRemoteIp /* = "" */, USHORT usRemotePort /* = 0 */)
{
	m_hRemoteSocket		 = hRemoteSocket;
	m_strRemoteIp		 = strRemoteIp;
	m_usRemotePort		 = usRemotePort;
	return TRUE;
}

BOOL AsyncSocketStream::Close()
{
	g_CloseSocket(m_hRemoteSocket);
	return TRUE;
}

BOOL AsyncSocketStream::UnInit()
{
	g_CloseSocket(m_hRemoteSocket);
	m_hRemoteSocket				  = INVALID_SOCKET;
	m_strRemoteIp				  = "";
	m_usRemotePort				  = 0;

	return TRUE;
}

INT AsyncSocketStream::Send(SPDynamicBuffer &spBuffer, INT *pErrorCode /* = NULL */)
{
	g_SetErrorCode(pErrorCode, 0);

	CHECK_RETURN_BOOL_QUIET(INVALID_SOCKET != m_hRemoteSocket && m_hRemoteSocket>0);

	WORD wDataSize = static_cast<WORD>(spBuffer->GetUsedSize());
	spBuffer->InsertDataIntoHead(&wDataSize, sizeof(WORD));

	PCHAR pPackage = static_cast<PCHAR>(spBuffer->GetDataPtr());
	CHECK_RETURN_BOOL(NULL != pPackage);

	size_t nPackageSize = spBuffer->GetUsedSize();
	while(nPackageSize > 0)
	{
		INT nRetCode = ::send(m_hRemoteSocket, pPackage, nPackageSize, 0);
		if (SOCKET_ERROR != nRetCode)
		{
			pPackage     += nRetCode;
			nPackageSize -= nRetCode;
			continue;
		}

		if (g_IsSocketCanRestore())
		{
			continue;
		}

		if (g_IsSocketWouldBlock())
		{
			ASSERT(FALSE);
			return FALSE;
		}
		// other error save to pErrorCode
		INT nLastError = g_GetSocketLastError();
		g_SetErrorCode(pErrorCode, nLastError);
		return FALSE;
	}
	return TRUE;
}
/*返回值 -1:出错 0:已断开连接 1:接收成功*/
INT AsyncSocketStream::Recv(SPDynamicBuffer &spBuffer, INT *pErrorCode /* = NULL */)
{
	g_SetErrorCode(pErrorCode, 0);

	INT		bResult	= -1;

	INT			nReCode		= 0;
	INT			nLeftPos	= 0;
	INT			nTotalLen	= 0;

	nTotalLen = spBuffer->GetTotalSize();

	if (INVALID_SOCKET == m_hRemoteSocket)
	{
		nReCode = INVALID_SOCKET;
		g_SetErrorCode(pErrorCode, nReCode);
		return bResult;
	}
	PCHAR pBuffer = static_cast<PCHAR>(spBuffer->GetLeftPtr());
	while (true)
	{
		nReCode = ::recv(m_hRemoteSocket, pBuffer, nTotalLen-nLeftPos, 0);
		// recv error
		if (SOCKET_ERROR == nReCode)
		{
			if (g_IsSocketCanRestore())
			{
				continue;
			}

			if (g_IsSocketWouldBlock())
			{ // no data or in processing
				bResult = 1;
				goto Exit0;
			}
			// other error code save to pErrorCode
			nReCode = g_GetSocketLastError();
			g_SetErrorCode(pErrorCode, nReCode);
			bResult = -1;
#ifdef PLATFORM_OS_WINDOWS
			// WSAEFAULT:The system detected an invalid pointer address in attempting to use a pointer argument in a call
			// WSAECONNRESET:An existing connection was forcibly closed by the remote host.
			if (WSAEFAULT == nReCode || WSAECONNRESET == nReCode)
			{
				bResult = 0;
			}
#else
#endif // PLANT_OS_WINDOWS
			g_CloseSocket(m_hRemoteSocket);
			goto Exit0;
		}
		else if (0 == nReCode)
		{ // nRecode == 0: client socket close
			bResult = 0;
			break;
		}

		nLeftPos += nReCode;
		spBuffer->SetUseSize(nLeftPos);

		pBuffer += nReCode;

	}

Exit0:
	return bResult;
}

STRING AsyncSocketStream::GetRemoteIp() CONST
{
	return m_strRemoteIp;
}

USHORT AsyncSocketStream::GetRemotePort() CONST
{
	return m_usRemotePort;
}

SOCKET AsyncSocketStream::GetSocket() CONST
{
	return m_hRemoteSocket;
}
