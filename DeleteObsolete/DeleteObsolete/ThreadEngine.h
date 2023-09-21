#pragma once
#include <cassert>

CRITICAL_SECTION g_csLog;
class CThreadEngine
{
public:
	CThreadEngine(void)
	{
		__hThread = NULL;
	}
	~CThreadEngine(void)
	{
		WaitRunEnd();
	}

	void Run()
	{
		if (__hThread)
		{
			assert(!_T("이미 Run 중인 Thread가 왜 있지?"));
			WaitRunEnd();
		}

		DWORD dwThreadID = 0;
		__hThread = CreateThread(NULL, 0, __ThreadFunc, this, NULL, &dwThreadID);
	}
	BOOL WaitRunEnd()
	{
		if (__hThread == NULL) 
			return TRUE;

		WaitForSingleObject(__hThread, INFINITE);
		
		LARGE_INTEGER UserTime3, KernelTime3;  // get time
		FILETIME CreationTime, ExitTime, KernalTime, UserTime;
		GetThreadTimes(__hThread, &CreationTime, &ExitTime, &KernalTime, &UserTime);
		UserTime3.LowPart = UserTime.dwLowDateTime;
		UserTime3.HighPart = UserTime.dwHighDateTime;
		KernelTime3.LowPart = KernalTime.dwLowDateTime;
		KernelTime3.HighPart = KernalTime.dwHighDateTime;
		timeUser = ((double)UserTime3.QuadPart) / 10000.; // ms
		timeKernal = ((double)KernelTime3.QuadPart) / 10000.;
		timeTotal = timeUser + timeKernal;

		CloseHandle(__hThread);
		__hThread = NULL;

		return __bResult;
	}

	virtual BOOL Process() = 0;
	double timeUser, timeKernal, timeTotal;

private:
	HANDLE	__hThread;
	BOOL	__bResult;

	static DWORD WINAPI __ThreadFunc(LPVOID arg)
	{
		CThreadEngine* pThis = (CThreadEngine*)arg;
		pThis->__bResult = pThis->Process();

		return 0;
	}
};

class CThreadEngine_ProcessingPaths : public CThreadEngine
{
public:
	std::vector<tstring> m_pathsHit;
	ENUM_MODE m_nMode;
	std::vector<tstring>* m_pLogs;

	virtual BOOL Process()
	{
		switch (m_nMode)
		{
		case ENUM_MODE::DEL:
			for (long cnt = 0; cnt < m_pathsHit.size(); cnt++)
			{
				DWORD ftyp = GetFileAttributesA(STRING(m_pathsHit[cnt]).c_str());
				BOOL bSuccess = FALSE;
				if (ftyp != INVALID_FILE_ATTRIBUTES && ftyp & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (!DeleteDirectory(m_pathsHit[cnt]))
						bSuccess = TRUE;
				}
				else if (ftyp != INVALID_FILE_ATTRIBUTES)
				{
					if (!std::remove(STRING(m_pathsHit[cnt]).c_str()))
						bSuccess = TRUE;
				}

				tstring strLine;
				if (bSuccess)
				{
					//_tprintf(_T("del : %s\n"), m_pathsHit[cnt].c_str());
					strLine = std::format(_T("del : {}\n"), m_pathsHit[cnt]);
				}
				else
				{
					//tprintf(_T("del_FAIL : %s\n"), m_pathsHit[cnt].c_str());
					strLine = std::format(_T("del_FAIL : {}\n"), m_pathsHit[cnt]);
				}
				EnterCriticalSection(&g_csLog);
				(*m_pLogs).push_back(strLine);
				LeaveCriticalSection(&g_csLog);
			}
			break;
		case ENUM_MODE::MOVE:
			break;
		case ENUM_MODE::BRING_BACK:
			break;
		case ENUM_MODE::BIN:
			break;
		case ENUM_MODE::BIN_EMPTY:
			break;
		default:;
		}

		return TRUE;
	}
};

class CThreadEngine_Log : public CThreadEngine
{
public:
	std::vector<tstring>* m_pLogs;
	HANDLE m_hKill;

	virtual BOOL Process()
	{
		while (1)
		{
			DWORD dwEvent = ::WaitForSingleObject(m_hKill, 500);

			std::vector<tstring> logs;

			EnterCriticalSection(&g_csLog);
			std::swap(logs, *m_pLogs);
			LeaveCriticalSection(&g_csLog);

			for (long cnt = 0; cnt < logs.size(); cnt++)
			{
				_tprintf(_T("%s"), logs[cnt].c_str());
			}

			if (dwEvent == WAIT_OBJECT_0)
			{
				_tprintf(_T("Logs Ended\n"));
				break;
			}
		}
		return TRUE;
	}
};