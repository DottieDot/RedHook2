#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tlhelp32.h>

#pragma warning(disable : 6387)
#pragma warning(disable : 6011)

DWORD GetProcessIdFromName(const WCHAR* pwszProcessName)
{
    DWORD          dwPid = 0;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(hSnapshot, &entry))
    {
        do
        {
            if (!_wcsicmp(entry.szExeFile, pwszProcessName))
            {
                dwPid = entry.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &entry));
    }

    CloseHandle(hSnapshot);
    return dwPid;
}

BOOL InjectDLL(DWORD dwPid, const WCHAR* pwszPath)
{
    WCHAR wszRealPath[MAX_PATH];

    DWORD dwPathLen = GetFullPathNameW(pwszPath, MAX_PATH, wszRealPath, NULL);

    if (dwPathLen == 0)
    {
        printf("ERROR: GetFullPathNameW(%u)", GetLastError());
        return FALSE;
    }

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);

    if (!hProc)
    {
        printf("ERROR: OpenProcess (%u)\n", GetLastError());
        return FALSE;
    }

    LPVOID lpAlloc =
        VirtualAllocEx(hProc, NULL, dwPathLen * sizeof(WCHAR), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (!lpAlloc)
    {
        printf("ERROR: VirtualAlloc (%u)\n", GetLastError());
        return FALSE;
    }

    BOOL bWriteOk =
        WriteProcessMemory(hProc, lpAlloc, wszRealPath, dwPathLen * sizeof(WCHAR), NULL);
    if (!bWriteOk)
    {
        printf("ERROR: WriteProcessMemory (%u)\n", GetLastError());
        return FALSE;
    }

    LPTHREAD_START_ROUTINE lpLoadLibrary =
        (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibraryA("kernel32"), "LoadLibraryW");

    if (!lpLoadLibrary)
    {
        printf("ERROR: GetProcAddress (%u)\n", GetLastError());
        return FALSE;
    }

    DWORD  dwThreadId = 0;
    HANDLE hThread    = CreateRemoteThread(hProc, NULL, 0, lpLoadLibrary, lpAlloc, 0, &dwThreadId);
    if (!hThread)
    {
        printf("ERROR: CreateRemoteThread (%u)\n", GetLastError());
        return FALSE;
    }

    return TRUE;
}

int main()
{
    printf("Waiting for RDR2.exe...\n");

    DWORD dwPid = 0;
    while (!(dwPid = GetProcessIdFromName(L"RDR2.exe"))) Sleep(200);

    printf("RDR2.exe found PID: %u\n", dwPid);

    if (InjectDLL(dwPid, L"RedHook2.dll"))
    {
        printf("RedHook2.dll loaded\n");
    }

    printf("Closing in 5 seconds\n");

    Sleep(5'000);

    return 0;
}
