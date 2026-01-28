#include <windows.h>

extern "C" __declspec(dllexport)
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        MessageBoxA(NULL, "Injection Successful!", "Test DLL", MB_OK | MB_ICONINFORMATION);
    }
    return TRUE;
}
