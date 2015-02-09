#include "SocketStream.h"

#ifdef PLATFORM_OS_LINUX
// return -1: error, 0:No Complete Package, 1:Success
INT AsyncSocketStream::Recv(SPDynamicBuffer &spBuffer, INT *pErrorCode)
{
	INT nResult = -1;
	INT nRetCode = 0;

	PROCESS_ERROR(INVALID_SOCKET != m_hRemoteSocket);

	PROCESS_ERROR(spBuffer);

	g_SetErrorCode(pErrorCode, 0);

//	TryEpollRecv();
 	// no data from client
 	CHECK_RETURN_CODE_QUIET(FALSE != m_bRecvCompletedFlag, 0);

    // inner system error occure
 	CHECK_RETURN_CODE_QUIET(0 == m_nRecvErrorCode, -1);
 
 	// need to close socket
 	CHECK_RETURN_CODE_QUIET(FALSE == m_bNeedToCloseFlag, 0);

	nRetCode = m_spDataBuffer->GetPackage(spBuffer);
	if (!nRetCode) // 没有完整的数据包
	{
		g_SetErrorCode(pErrorCode, 0);
		nResult = 0;
	}
	else
	{
		g_SetErrorCode(pErrorCode, 1);
		nResult = 1;
	}

Exit0:
	return nResult;
}

// return -1: Error, 0:Close, 1:Success
INT AsyncSocketStream::TryEpollRecv()
{
    INT nRetCode = 0, nResult = -1;
	size_t nMaxLen = 0;
    PCHAR pBuffer = NULL;

	m_nRecvErrorCode = 0;
	m_bNeedToCloseFlag = FALSE;
    m_bRecvCompletedFlag = FALSE;
    while(true)
    {
        nMaxLen = m_spDataBuffer->GetTotalSize() - m_spDataBuffer->GetUsedSize();
		if (nMaxLen == 0)
			break;

        pBuffer = (PCHAR)m_spDataBuffer->GetLeftPtr();
		nRetCode = ::recv(m_hRemoteSocket, pBuffer, nMaxLen, 0);
		if (SOCKET_ERROR == nRetCode)
		{
			if (g_IsSocketCanRestore())
				continue;

			if (g_IsSocketWouldBlock())
			{
				nRetCode = 1;
				PROCESS_ERROR_QUIET(FALSE);
			}
			// inner error
            nResult = -1;
			m_bNeedToCloseFlag = TRUE;
			m_nRecvErrorCode = g_GetSocketLastError();
			PROCESS_ERROR_QUIET(FALSE);
		}
		else if (0 == nRetCode)
		{
            nResult = 0;
			m_bNeedToCloseFlag = TRUE;
			break;
		}

		m_spDataBuffer->AddUsedSize(nRetCode);
    }

Exit0:
	if (nRetCode > 0)
	{
        nResult = 1;
		m_bRecvCompletedFlag = TRUE;
	}
    return nResult;
}

#endif // PLATFORM_OS_WINDOWS
