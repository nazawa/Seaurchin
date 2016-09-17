#include "Main.h"
#include "Debug.h"
#include "Setting.h"


void PreInitialize(HINSTANCE hInstance);
void Initialize();
void Run();
void Terminate();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    PreInitialize(hInstance);
    Initialize();

    Run();

    Terminate();
    return 0;				
}

void PreInitialize(HINSTANCE hInstance)
{
    InitializeDebugFeature();
    InitializeSetting(hInstance);
    ChangeWindowMode(TRUE);
    SetMainWindowText(SU_APP_NAME " " SU_APP_VERSION);
    SetGraphMode(SU_RES_WIDTH, SU_RES_HEIGHT, 32);
}

void Initialize()
{
    if (DxLib_Init() == -1) abort();
    WriteDebugConsole(TEXT("DxLib_Init\n"));

    SettingLoadSetting();
}

void Run()
{
    while (ProcessMessage() != -1)
    {

    }
}

void Terminate()
{
    TerminateDebugFeature();
    DxLib_End();
}