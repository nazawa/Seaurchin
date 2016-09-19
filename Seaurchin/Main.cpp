#include "Main.h"
#include "Debug.h"
#include "Setting.h"
#include "Execution.h"
#include "SceneDebug.h"

using namespace std;
using namespace std::chrono;

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
    SetAlwaysRunFlag(TRUE);
    SetWaitVSyncFlag(FALSE);
    SetGraphMode(SU_RES_WIDTH, SU_RES_HEIGHT, 32);
}

void Initialize()
{
    if (DxLib_Init() == -1) abort();
    SetDrawScreen(DX_SCREEN_BACK);

    WriteDebugConsole(TEXT("DxLib_Init\n"));

    SettingLoadSetting();
    ExecutionEnumerateSkins();
    ExecutionExecute();
    ExecutionAddScene(shared_ptr<Scene>(new SceneDebug()));
    
}

void Run()
{
    auto start = high_resolution_clock::now();
    auto pstart = start;
    while (ProcessMessage() != -1)
    {
        pstart = start;
        start = high_resolution_clock::now();
        float delta = duration_cast<nanoseconds>(start - pstart).count() / 1000000000.0;
        ExecutionTick(delta);
        ExecutionDraw();
    }
}

void Terminate()
{
    TerminateDebugFeature();
    DxLib_End();
}
