#include <cstdio>
#include <tchar.h>
#include <windows.h>

#include <locale.h>

int main()
{
    _tsetlocale(LC_ALL, _T("korean"));
    _tprintf(_T("ppppp\n"));

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    TCHAR    commandLine[] = _T("DeleteGratuitous.exe");

    DWORD timePin;
    timePin = GetTickCount64();

    // Start the child process. 
    if (!CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        _tprintf(_T("CreateProcess failed (%d).\n"), GetLastError());
        return 0;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);


    /*
    double UserSeconds, KernalSeconds;
    LARGE_INTEGER UserTime3, KernelTime3;
    FILETIME CreationTime, ExitTime, KernalTime, UserTime;

        GetThreadTimes(pi.hProcess, &CreationTime, &ExitTime, &KernalTime, &UserTime);
        UserTime3.LowPart = UserTime.dwLowDateTime;
        UserTime3.HighPart = UserTime.dwHighDateTime;
        KernelTime3.LowPart = KernalTime.dwLowDateTime;
        KernelTime3.HighPart = KernalTime.dwHighDateTime;

        UserSeconds = ((double)UserTime3.QuadPart) / 10000000.;
        KernalSeconds = ((double)KernelTime3.QuadPart) / 10000.;

    _tprintf(_T("UserSeconds : \n"));
        _tprintf(_T("	%.1f [s]\n"), UserSeconds);
    _tprintf(_T("KernalSeconds : \n"));
        _tprintf(_T("	%.1f [ms]\n"), KernalSeconds);

     double   TotalSeconds = ((double)KernelTime3.QuadPart + (double)UserTime3.QuadPart) / 10000000.;
        _tprintf(_T("TotalSeconds : \n"));
        _tprintf(_T("	%.1f [ms]\n"), TotalSeconds);
        */

        _tprintf(_T("TotalSeconds : \n"));
        _tprintf(_T("	%.4f [s]\n"), (GetTickCount64() - timePin) / 1000.);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    int temp = getchar();
    return 0;
}