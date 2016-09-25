#include "ExecutionManager.h"

#include "Config.h"
#include "Debug.h"
#include "Setting.h"
#include "Interfaces.h"

using namespace boost::filesystem;
using namespace std;

ExecutionManager::ExecutionManager(std::shared_ptr<Setting> setting)
{
    ScriptInterface = shared_ptr<AngelScript>(new AngelScript());
    InterfacesRegisterEnum(ScriptInterface->GetEngine());
    InterfacesRegisterSprite(ScriptInterface->GetEngine());
    InterfacesRegisterScene(ScriptInterface->GetEngine());
    InterfacesRegisterGlobalFunction(ScriptInterface->GetEngine());
    InterfacesRegisterSceneFunction(ScriptInterface->GetEngine());

    SharedSetting = setting;
    SharedKeyState = shared_ptr<KeyState>(new KeyState());
}

void ExecutionManager::EnumerateSkins()
{
    using namespace boost;
    using namespace boost::filesystem;
    using namespace boost::xpressive;

    path sepath = Setting::GetRootDirectory() / SU_DATA_DIR / SU_SKIN_DIR;

    for (const auto& fdata : make_iterator_range(directory_iterator(sepath), {}))
    {
        if (!is_directory(fdata)) continue;
        if (!CheckSkinStructure(fdata.path())) continue;
        SkinNames.push_back(fdata.path().filename().string());
    }
    ostringstream ss;
    ss << "Found " << SkinNames.size() << " Skins" << endl;
    WriteDebugConsole(ss.str().c_str());
}

void ExecutionManager::InitializeExecution()
{
    //StartSystemMenu();
    auto sn = SharedSetting->ReadValue<string>(SU_SETTING_GENERAL, SU_SETTING_SKIN, "Default");
    if (find(SkinNames.begin(), SkinNames.end(), sn) == SkinNames.end())
    {
        WriteDebugConsole(("Can't Find Skin " + sn + "!\n").c_str());
        return;
    }
    Skin = unique_ptr<SkinHolder>(new SkinHolder(sn, this));
    Skin->Initialize();
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
    scene->SetManager(this);
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

shared_ptr<ScriptScene> ExecutionManager::CreateSceneFromScriptObject(asIScriptObject *obj)
{
    shared_ptr<ScriptScene> ret;
    auto type = obj->GetObjectType();
    if (ScriptInterface->CheckImplementation(type, SU_IF_COSCENE))
    {
        return shared_ptr<ScriptScene>(new ScriptCoroutineScene(obj));
    }
    else if (ScriptInterface->CheckImplementation(type, SU_IF_SCENE))  //最後
    {
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

    path sysmf = Setting::GetRootDirectory() / SU_DATA_DIR / SU_SCRIPT_DIR / SU_SYSTEM_MENU_FILE;
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
    if (!exists(name / SU_SCRIPT_DIR / SU_SKIN_TITLE_FILE)) return false;
    if (!exists(name / SU_SCRIPT_DIR / SU_SKIN_SELECT_FILE)) return false;
    if (!exists(name / SU_SCRIPT_DIR / SU_SKIN_PLAY_FILE)) return false;
    if (!exists(name / SU_SCRIPT_DIR / SU_SKIN_RESULT_FILE)) return false;
    return true;
}


// SkinHolder -----------------------------

bool SkinHolder::IncludeScript(std::string include, std::string from, CScriptBuilder * builder)
{
    return false;
}

SkinHolder::SkinHolder(string name, ExecutionManager *manager)
{
    Manager = manager;
    SkinName = name;
    SkinRoot = Setting::GetRootDirectory() / SU_DATA_DIR / SU_SKIN_DIR / SkinName;
}

SkinHolder::~SkinHolder()
{

}

void SkinHolder::AddRef()
{
}

void SkinHolder::Release()
{
}

void SkinHolder::Initialize()
{
    auto si = Manager->GetScriptInterface();
    si->StartBuildModule("SkinLoader",
        [this](string inc, string from, CScriptBuilder *b)
    {
        if (!exists(SkinRoot / SU_SCRIPT_DIR / inc)) return false;
        b->AddSectionFromFile((SkinRoot / SU_SCRIPT_DIR / inc).string().c_str());
        return true;
    });
    si->LoadFile((SkinRoot / SU_SKIN_MAIN_FILE).string().c_str());
    si->FinishBuildModule();

    auto mod = si->GetLastModule();
    int fc = mod->GetFunctionCount();
    asIScriptFunction *ep = nullptr;
    for (asUINT i = 0; i < fc; i++)
    {
        auto func = mod->GetFunctionByIndex(i);
        if (!si->CheckMetaData(func, "EntryPoint")) continue;
        ep = func;
        break;
    }
    if (!ep)
    {
        WriteDebugConsole("Entry Point Not Found!\n");
        mod->Discard();
        return;
    }

    auto ctx = si->GetEngine()->CreateContext();
    ctx->Prepare(ep);
    ctx->SetArgObject(0, this);
    ctx->Execute();
    ctx->Release();
    mod->Discard();
}

void SkinHolder::LoadSkinImage(const string &key, const string &filename)
{
    Images[key] = Image::LoadFromFile((SkinRoot / SU_IMAGE_DIR / filename).string().c_str());
}

void SkinHolder::LoadSkinFont(const string &key, const string &filename)
{
    Fonts[key] = Font::LoadFromFile((SkinRoot / SU_FONT_DIR / filename).string().c_str());
}

shared_ptr<Image> SkinHolder::GetSkinImage(const string &key)
{
    return Images[key];
}

shared_ptr<Font> SkinHolder::GetSkinFont(const string &key)
{
    return Fonts[key];
}