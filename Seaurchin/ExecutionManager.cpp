#include "ExecutionManager.h"
#include "Config.h"
#include "Debug.h"
#include "Setting.h"
#include "AngelScriptManager.h"

using namespace std;

ExecutionManager::ExecutionManager()
{
    ScriptInterface = shared_ptr<AngelScript>(new AngelScript());

    Shared = shared_ptr<SharedInfo>(new SharedInfo());
    SharedKeyState = shared_ptr<KeyState>(new KeyState());
    Shared->Key = SharedKeyState;
}

void ExecutionManager::EnumerateSkins()
{
    using namespace boost;
    using namespace boost::filesystem;
    using namespace boost::xpressive;

    path sepath = SettingGetRootDirectory() / SU_DATA_DIR / SU_SKIN_DIR;

    for (const auto& fdata : make_iterator_range(directory_iterator(sepath), {}))
    {
        if (!is_directory(fdata)) continue;
        if (!CheckSkinStructure(fdata.path())) continue;
        WriteDebugConsole(("Skin Found:" + fdata.path().filename().string() + "\n").c_str());
    }
}

void ExecutionManager::InitializeExecution()
{
    StartSystemMenu();
}

//Tick
void ExecutionManager::Tick(double delta)
{
    memcpy_s(SharedKeyState->Last, 256, SharedKeyState->Current, 256);
    GetHitKeyStateAll(SharedKeyState->Current);
    for (int i = 0; i < 256; i++) SharedKeyState->Trigger[i] = !SharedKeyState->Last[i] && SharedKeyState->Current[i];

    sort(Scenes.begin(), Scenes.end(), [](shared_ptr<Scene> sa, shared_ptr<Scene> sb) { return sa->GetIndex() < sb->GetIndex(); });
    auto i = Scenes.begin();
    while (i != Scenes.end())
    {
        (*i)->Tick(delta);
        if ((*i)->IsDead())
        {
            i = Scenes.erase(i);
        }
        else
        {
            i++;
        }
    }
}

//Draw
void ExecutionManager::Draw()
{
    ClearDrawScreen();
    for (const auto& s : Scenes) s->Draw();
    ScreenFlip();
}

void ExecutionManager::AddScene(shared_ptr<Scene> scene)
{
    Scenes.push_back(scene);
    scene->SetSharedInfo(Shared);
    scene->Initialize();
}

shared_ptr<ScriptScene> ExecutionManager::CreateSceneFromScriptType(asITypeInfo *type)
{
    shared_ptr<ScriptScene> ret;
    if (ScriptInterface->CheckImplementation(type, SU_IF_COSCENE))
    {
        auto obj = ScriptInterface->InstantiateObject(type);
        return shared_ptr<ScriptScene>(new ScriptCoroutineScene(obj));
    }
    else if (ScriptInterface->CheckImplementation(type, SU_IF_SCENE))  //最後
    {
        auto obj = ScriptInterface->InstantiateObject(type);
        return shared_ptr<ScriptScene>(new ScriptScene(obj));
    }
    else
    {
        ostringstream err;
        err << "Type '" << type->GetName() << "' Doesn't Implement any Scene Interface!\n" << endl;
        WriteDebugConsole(err.str().c_str());
        return nullptr;
    }
}

void ExecutionManager::StartSystemMenu()
{
    using namespace boost;
    using namespace boost::filesystem;

    path sysmf = SettingGetRootDirectory() / SU_DATA_DIR / SU_SCRIPT_DIR / SU_SYSTEM_MENU_FILE;
    if (!exists(sysmf))
    {
        WriteDebugConsole("System Menu Script Not Found!\n");
        return;
    }

    ScriptInterface->StartBuildModule("SystemMenu", [](auto inc, auto from, auto sb) { return true; });
    ScriptInterface->LoadFile(sysmf.string().c_str());
    if (!ScriptInterface->FinishBuildModule())
    {
        WriteDebugConsole("Can't Comple System Menu!\n");
        return;
    }
    auto mod = ScriptInterface->GetLastModule();
    
    //エントリポイント検索
    int cnt = mod->GetObjectTypeCount();
    asITypeInfo *type = nullptr;
    for (int i = 0; i < cnt; i++)
    {
        auto cti = mod->GetObjectTypeByIndex(i);
        if (!ScriptInterface->CheckMetaData(cti, "EntryPoint")) continue;
        type = cti;
        type->AddRef();
        break;
    }
    if (!type)
    {
        WriteDebugConsole("Entry Point Not Found!\n");
        return;
    }

    AddScene(CreateSceneFromScriptType(type));

    type->Release();
}

bool ExecutionManager::CheckSkinStructure(boost::filesystem::path name)
{
    using namespace boost;
    using namespace boost::filesystem;

    if (!exists(name / SU_SKIN_MAIN_FILE)) return false;

    return true;
}