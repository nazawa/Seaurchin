#include "Main.h"
#include "Debug.h"
#include "Setting.h"
#include "ExecutionManager.h"
#include "SceneDebug.h"

using namespace std;
using namespace std::chrono;

void PreInitialize(HINSTANCE hInstance);
void Initialize();
void Run();
void Terminate();

shared_ptr<Setting> setting;
unique_ptr<ExecutionManager> manager;


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
    setting = shared_ptr<Setting>(new Setting(hInstance));
    manager = unique_ptr<ExecutionManager>(new ExecutionManager(setting));

    InitializeDebugFeature();
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

    setting->Load(SU_SETTING_FILE);
    manager->EnumerateSkins();
    //manager->ExecuteSkin();
    manager->ExecuteSystemMenu();
    manager->AddScene(shared_ptr<Scene>(new SceneDebug()));
    
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
        manager->Tick(delta);
        manager->Draw();
    }
}

void Terminate()
{
    TerminateDebugFeature();
    setting->Save();
    DxLib_End();
}
