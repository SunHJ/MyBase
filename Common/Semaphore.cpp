#include "Global.h"
#include "Semaphore.h"

Semaphore::Semaphore(LONG nInitNum /* = 0 */, LONG nMaxNum /* = SEMAPHORE_MAX_NUM */)
{
	m_hSemaphore = ::CreateSemaphore(NULL, nInitNum, nMaxNum, NULL);
	while (!g_CheckHandle(m_hSemaphore))
	{
		ASSERT(FALSE);
		m_hSemaphore = ::CreateSemaphore(NULL, nInitNum, nMaxNum, NULL);
	}
}

Semaphore::~Semaphore()
{
	g_CloseHandle(m_hSemaphore);
}

BOOL Semaphore::ReleaseSemaphore(LONG nReleaseNum /* = 1 */)
{
	CHECK_RETURN_BOOL(NULL!=m_hSemaphore && INVALID_HANDLE_VALUE != m_hSemaphore);
	return ::ReleaseSemaphore(m_hSemaphore, nReleaseNum, NULL);
}

BOOL Semaphore::WaitSemaphore(DWORD dwMilliseconds /* = INFINITE */)
{
	CHECK_RETURN_BOOL(NULL!=m_hSemaphore && INVALID_HANDLE_VALUE != m_hSemaphore);
	return WAIT_OBJECT_0 == ::WaitForSingleObject(m_hSemaphore, dwMilliseconds);
}
