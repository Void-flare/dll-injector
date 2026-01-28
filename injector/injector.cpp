#include <windows.h>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include "process_utils.h"

static bool is_number(const std::wstring& s) {
    if (s.empty()) return false;
    for (wchar_t c : s) {
        if (c < L'0' || c > L'9') return false;
    }
    return true;
}

static std::wstring to_absolute(const std::wstring& path) {
    try {
        return std::filesystem::absolute(std::filesystem::path(path)).wstring();
    } catch (...) {
        return path;
    }
}

static DWORD resolve_pid(const std::wstring& target) {
    if (is_number(target)) return static_cast<DWORD>(std::stoul(target));
    return FindProcessIdByName(target);
}

static DWORD inject(DWORD pid, const std::wstring& dllPath) {
    std::wstring abs = to_absolute(dllPath);
    HANDLE process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pid);
    if (!process) return GetLastError();
    SIZE_T bytes = (abs.size() + 1) * sizeof(wchar_t);
    LPVOID remote = VirtualAllocEx(process, nullptr, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remote) {
        DWORD e = GetLastError();
        CloseHandle(process);
        return e;
    }
    SIZE_T written = 0;
    if (!WriteProcessMemory(process, remote, abs.c_str(), bytes, &written)) {
        DWORD e = GetLastError();
        VirtualFreeEx(process, remote, 0, MEM_RELEASE);
        CloseHandle(process);
        return e;
    }
    HMODULE k32 = GetModuleHandleW(L"kernel32.dll");
    FARPROC loadlib = GetProcAddress(k32, "LoadLibraryW");
    if (!loadlib) {
        DWORD e = GetLastError();
        VirtualFreeEx(process, remote, 0, MEM_RELEASE);
        CloseHandle(process);
        return e;
    }
    HANDLE thread = CreateRemoteThread(process, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(loadlib), remote, 0, nullptr);
    if (!thread) {
        DWORD e = GetLastError();
        VirtualFreeEx(process, remote, 0, MEM_RELEASE);
        CloseHandle(process);
        return e;
    }
    WaitForSingleObject(thread, INFINITE);
    DWORD exitCode = 0;
    GetExitCodeThread(thread, &exitCode);
    CloseHandle(thread);
    VirtualFreeEx(process, remote, 0, MEM_RELEASE);
    CloseHandle(process);
    return exitCode;
}

int wmain(int argc, wchar_t* argv[]) {
    if (argc < 3) {
        std::wcerr << L"Usage: injector <pid|process_name.exe> <path_to_dll>\n";
        return 1;
    }
    std::wstring target = argv[1];
    std::wstring dll = argv[2];
    DWORD pid = resolve_pid(target);
    if (!pid) {
        std::wcerr << L"Target not found\n";
        return 2;
    }
    DWORD result = inject(pid, dll);
    if (result == 0) {
        std::wcout << L"Injection successful\n";
        return 0;
    } else {
        std::wcerr << L"Injection failed: " << result << L"\n";
        return 3;
    }
}
