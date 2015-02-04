#include "SocketStream.h"

#ifdef PLATFORM_OS_WINDOWS
// return -1: error, 0:NoData, 1:success
INT AsyncSocketStream::Recv(SPDynamicBuffer &spBuffer, INT *pErrorCode)
{
	INT nResult = -1;
	INT nRetCode = 0;

	PROCESS_ERROR(INVALID_SOCKET != m_hRemoteSocket);
	PROCESS_ERROR(spBuffer);

	g_SetErrorCode(pErrorCode, 0);

	// no data from client
	CHECK_RETURN_CODE_QUIET(FALSE != m_bRecvCompletedFlag, 0);

	// inner system error occure
	CHECK_RETURN_CODE_QUIET(0 == m_nRecvErrorCode, -1);

	// need to close socket
	CHECK_RETURN_CODE_QUIET(FALSE == m_bNeedToCloseFlag, 0);

	m_spDataBuffer->AddUsedSize(m_nRecvSize);
	nRetCode = m_spDataBuffer->GetPackage(spBuffer);
	if (!nRetCode) // û�����������ݰ�
	{
		g_SetErrorCode(pErrorCode, 0);
		nResult = 0;
	}
	else
	{
		g_SetErrorCode(pErrorCode, 1);
		nResult = 1;
	}

	// ������һ��Ͷ��
	nRetCode = TryToActiveNextRecv();
	if (nRetCode == -1)
	{
		nRetCode = g_GetSocketLastError();
		g_SetErrorCode(pErrorCode, nRetCode);

		m_bNeedToCloseFlag = TRUE;
		nResult = -1;
	}

Exit0:
	return nResult;
}

// return -1: Error, 0:NoData, 1:Success
INT AsyncSocketStream::TryToActiveNextRecv()
{
	INT nResult = 0, nRetCode;
	DWORD dwRecv, dwFlags;
	BOOL bLoopFlag = TRUE;
	while (bLoopFlag)
	{
		m_wsaBuffer.len = m_spDataBuffer->GetTotalSize() - m_spDataBuffer->GetUsedSize();
		m_wsaBuffer.buf = (PCHAR)m_spDataBuffer->GetLeftPtr();	 

		g_ZeroMemory(&m_wsaOverlapped, sizeof(m_wsaOverlapped));
		m_nRecvSize = 0;
		m_bRecvCompletedFlag = FALSE;
		m_nRecvErrorCode = 0;
		dwRecv = 0;
		dwFlags = 0;
		nRetCode = ::WSARecv(m_hRemoteSocket, &m_wsaBuffer, 1, &dwRecv, &dwFlags, &m_wsaOverlapped, NULL);
		if (nRetCode < 0)
		{
			nRetCode = g_IsSocketCanRestore();
			if (nRetCode)
				continue;

			nRetCode = g_IsSocketWouldBlock();
			if (nRetCode)
			{
				nResult = 0;
				PROCESS_ERROR_QUIET(FALSE);
			}
			// WSARecv ���ִ�����Ҫ�ر��׽���
			m_bNeedToCloseFlag = TRUE;
			// ���������껺������δ����������
			m_bRecvCompletedFlag = TRUE;
			nResult = -1;
			PROCESS_ERROR(FALSE);
		}
		break;
	}
	nResult = 1;

Exit0:
	return nResult;
}

BOOL AsyncSocketStream::OnRecvCompleted(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
	UNREFERENCED_PARAMETER(lpOverlapped);
	m_nRecvErrorCode = dwErrorCode;
	m_nRecvSize = dwNumberOfBytesTransfered;
	m_bRecvCompletedFlag = TRUE;
	return TRUE;
}

#endif // PLATFORM_OS_WINDOWS
