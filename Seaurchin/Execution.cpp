#include "Execution.h"
#include "Config.h"
#include "Debug.h"
#include "Setting.h"
#include "SceneManager.h"

using namespace std;
shared_ptr<SceneManager> manager;

static bool ExecutionCheckSkinStructure(boost::filesystem::path name);

void ExecutionEnumerateSkins()
{
    using namespace boost;
    using namespace boost::filesystem;
    using namespace boost::xpressive;

    path sepath = SettingGetRootDirectory() / SU_DATA_DIR / SU_SKIN_DIR;

    for (const auto& fdata : make_iterator_range(directory_iterator(sepath), {}))
    {
        if (!is_directory(fdata)) continue;
        if (!ExecutionCheckSkinStructure(fdata.path())) continue;
        WriteDebugConsole(("Skin Found:" + fdata.path().filename().string() + "\n").c_str());
    }
}

static bool ExecutionCheckSkinStructure(boost::filesystem::path name)
{
    using namespace boost;
    using namespace boost::filesystem;

    if (!exists(name / SU_SKIN_MAIN_FILE)) return false;

    return true;
}

void ExecutionExecute()
{
    manager = shared_ptr<SceneManager>(new SceneManager());
}

//Tick
void ExecutionTick(double delta)
{
    manager->Tick(delta);
}

//Draw
void ExecutionDraw()
{
    ClearDrawScreen();
    manager->Draw();
    ScreenFlip();
}

void ExecutionAddScene(std::shared_ptr<Scene> scene)
{
    manager->AddScene(scene);
}

shared_ptr<SceneManager> ExecutionGetManager()
{
    return manager;
}
