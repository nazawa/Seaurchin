#include "Main.h"
#include "resource.h"
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
LRESULT CALLBACK CustomWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

shared_ptr<Setting> setting;
unique_ptr<ExecutionManager> manager;
WNDPROC dxlibWndProc;
HWND hDxlibWnd;

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

    InitializeDebugFeature();
    ChangeWindowMode(TRUE);
    SetMainWindowText(SU_APP_NAME " " SU_APP_VERSION);
    SetAlwaysRunFlag(TRUE);
    SetWaitVSyncFlag(FALSE);
    SetWindowIconID(IDI_ICON1);
    SetGraphMode(SU_RES_WIDTH, SU_RES_HEIGHT, 32);
}

void Initialize()
{
    if (DxLib_Init() == -1) abort();
    WriteDebugConsole(TEXT("DxLib_Init\n"));
    //WndProc·‚µ‘Ö‚¦
    hDxlibWnd = GetMainWindowHandle();
    dxlibWndProc = (WNDPROC)GetWindowLong(hDxlibWnd, GWL_WNDPROC);
    SetWindowLong(hDxlibWnd, GWL_WNDPROC, (LONG)CustomWindowProc);
    //D3DÝ’è
    SetDrawScreen(DX_SCREEN_BACK);
    SetChangeScreenModeGraphicsSystemResetFlag(FALSE);
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);

    setting->Load(SU_SETTING_FILE);
    manager = unique_ptr<ExecutionManager>(new ExecutionManager(setting));
}

void Run()
{
    if (CheckHitKey(KEY_INPUT_F2))
    {
        manager->ExecuteSystemMenu();
    }
    else
    {
        manager->EnumerateSkins();
        manager->ExecuteSkin();
    }
    manager->AddScene(shared_ptr<Scene>(new SceneDebug()));


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

LRESULT CALLBACK CustomWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    bool processed;
    LRESULT result;
    tie(processed, result) = manager->CustomWindowProc(hWnd, msg, wParam, lParam);
    
    if (processed) {
        return result;
    }
    else {
        return CallWindowProc(dxlibWndProc, hWnd, msg, wParam, lParam);
    }
}
