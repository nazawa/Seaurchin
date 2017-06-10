#pragma once


#include "Config.h"

#ifdef _DEBUG
#ifdef UNICODE

#define WriteDebugConsole _WriteDebugConsoleW

#else // UNICODE、↓はマルチバイト時

#define WriteDebugConsole _WriteDebugConsoleA

#endif // UNICODE

#define InitializeDebugFeature _InitializeDebugFeature
#define TerminateDebugFeature _TerminateDebugFeature
#define WriteDebugConsoleU _WriteDebugConsoleU

#define ASSERT_CALL WriteDebugConsole((string(__func__) + " Called!\n").c_str())

#else // _DEBUG、↓はRelease時

#define WriteDebugConsole Debug_ReleaseFunction
#define InitializeDebugFeature Debug_ReleaseFunction
#define TerminateDebugFeature Debug_ReleaseFunction
#define WriteDebugConsoleU Debug_ReleaseFunction
#define ASSERT_CALL

#endif // _DEBUG

void _InitializeDebugFeature();
void _TerminateDebugFeature();

void _WriteDebugConsoleA(LPCSTR string);
void _WriteDebugConsoleW(LPCWSTR string);
void _WriteDebugConsoleU(const std::string& message);
void Debug_ReleaseFunction(...);