#include "Debug.h"

using namespace std;

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

//AngelScript—p
void _WriteDebugConsoleU(const string& message)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, nullptr, 0);
    wchar_t *buffer = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, buffer, len);
    WriteConsoleW(hConsoleOutput, buffer, wcslen(buffer), &dwDummy, nullptr);
    delete[] buffer;
}

void Debug_ReleaseFunction(...)
{
    
}