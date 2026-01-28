#pragma once
#include <string>
#include <windows.h>

DWORD FindProcessIdByName(const std::wstring& processName);
