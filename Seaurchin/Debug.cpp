#include "Debug.h"

static HANDLE   hConsoleOutput = nullptr,
                hConsoleInput = nullptr,
                hConsoleError = nullptr;

static DWORD dwDummy;

void _InitializeDebugFeature()
{
    AllocConsole();
    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
    hConsoleError = GetStdHandle(STD_ERROR_HANDLE);
}

void _TerminateDebugFeature()
{
    FreeConsole();
}

void _WriteDebugConsoleA(LPCSTR string)
{
    WriteConsoleA(hConsoleOutput, string, strlen(string), &dwDummy, nullptr);
}

void _WriteDebugConsoleW(LPCWSTR string)
{
    WriteConsoleW(hConsoleOutput, string, wcslen(string), &dwDummy, nullptr);
}

void Debug_ReleaseFunction(...)
{
    
}