#include "SkinHolder.h"
#include "Setting.h"
#include "ExecutionManager.h"

using namespace std;
using namespace boost::filesystem;

bool SkinHolder::IncludeScript(std::string include, std::string from, CScriptBuilder * builder)
{
    return false;
}

SkinHolder::SkinHolder(const wstring &name, shared_ptr<AngelScript> script, std::shared_ptr<SoundManager> sound)
{
    ScriptInterface = script;
	SoundInterface = sound;
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
    ScriptInterface->StartBuildModule("SkinLoader",
        [this](string inc, string from, CScriptBuilder *b)
    {
        if (!exists(SkinRoot / SU_SCRIPT_DIR / inc)) return false;
        b->AddSectionFromFile((SkinRoot / SU_SCRIPT_DIR / inc).string().c_str());
        return true;
    });
    ScriptInterface->LoadFile((SkinRoot / SU_SKIN_MAIN_FILE).string().c_str());
    ScriptInterface->FinishBuildModule();

    auto mod = ScriptInterface->GetLastModule();
    int fc = mod->GetFunctionCount();
    asIScriptFunction *ep = nullptr;
    for (asUINT i = 0; i < fc; i++)
    {
        auto func = mod->GetFunctionByIndex(i);
        if (!ScriptInterface->CheckMetaData(func, "EntryPoint")) continue;
        ep = func;
        break;
    }
    if (!ep)
    {
        WriteDebugConsole("Entry Point Not Found!\n");
        mod->Discard();
        return;
    }

    auto ctx = ScriptInterface->GetEngine()->CreateContext();
    ctx->Prepare(ep);
    ctx->SetArgObject(0, this);
    ctx->Execute();
    ctx->Release();
    mod->Discard();
}

asIScriptObject* SkinHolder::ExecuteSkinScript(const wstring &file)
{
    //お茶を濁せ
    auto modulename = ConvertUnicodeToUTF8(file);
    auto mod = ScriptInterface->GetExistModule(modulename);
    if (!mod)
    {
        ScriptInterface->StartBuildModule(modulename.c_str(),
            [this](string inc, string from, CScriptBuilder *b)
        {
            if (!exists(SkinRoot / SU_SCRIPT_DIR / inc)) return false;
            b->AddSectionFromFile((SkinRoot / SU_SCRIPT_DIR / inc).string().c_str());
            return true;
        });
        ScriptInterface->LoadFile((SkinRoot / SU_SCRIPT_DIR / file).string().c_str());
        if (!ScriptInterface->FinishBuildModule()) {
            ScriptInterface->GetLastModule()->Discard();
            return nullptr;
        }
        mod = ScriptInterface->GetLastModule();
    }

    //エントリポイント検索
    int cnt = mod->GetObjectTypeCount();
    asITypeInfo *type = nullptr;
    for (int i = 0; i < cnt; i++)
    {
        // ScriptBuilderのMetaDataのテーブルは毎回破棄されるので
        // asITypeInfoに情報を保持
        auto cti = mod->GetObjectTypeByIndex(i);
        if (!(ScriptInterface->CheckMetaData(cti, "EntryPoint") || cti->GetUserData(SU_UDTYPE_ENTRYPOINT))) continue;
        type = cti;
        type->SetUserData((void*)0xFFFFFFFF, SU_UDTYPE_ENTRYPOINT);
        type->AddRef();
        break;
    }
    if (!type)
    {
        WriteDebugConsole("Entry Point Not Found!\n");
        return nullptr;
    }

    auto obj = ScriptInterface->InstantiateObject(type);
    obj->SetUserData(this, SU_UDTYPE_SKIN);
    type->Release();
    return obj;
}

void SkinHolder::LoadSkinImage(const string &key, const string &filename)
{
    Images[key] = SImage::CreateLoadedImageFromFile(ConvertUnicodeToUTF8((SkinRoot / SU_IMAGE_DIR / filename).wstring()), false);
}

void SkinHolder::LoadSkinFont(const string &key, const string &filename)
{
    Fonts[key] = SFont::CreateLoadedFontFromFile(ConvertUnicodeToUTF8((SkinRoot / SU_FONT_DIR / filename).wstring()));
}

void SkinHolder::LoadSkinSound(const std::string & key, const std::string & filename)
{
	Sounds[key] = SSound::CreateSoundFromFile(SoundInterface.get(), ConvertUnicodeToUTF8((SkinRoot / SU_SOUND_DIR / filename).wstring()), 8);
}

void SkinHolder::LoadSkinAnime(const std::string & key, const std::string & filename, int x, int y, int w, int h, int c, double time)
{
    AnimatedImages[key] = SAnimatedImage::CreateLoadedImageFromFile(ConvertUnicodeToUTF8((SkinRoot / SU_IMAGE_DIR / filename).wstring()), x, y, w, h, c, time);
}

SImage* SkinHolder::GetSkinImage(const string &key)
{
    auto it = Images.find(key);
    if (it == Images.end()) return nullptr;
    it->second->AddRef();
    return it->second;
}

SFont* SkinHolder::GetSkinFont(const string &key)
{
    auto it = Fonts.find(key);
    if (it == Fonts.end()) return nullptr;
    it->second->AddRef();
    return it->second;
}

SSound* SkinHolder::GetSkinSound(const std::string & key)
{
	auto it = Sounds.find(key);
	if (it == Sounds.end()) return nullptr;
	it->second->AddRef();
	return it->second;
}

SAnimatedImage * SkinHolder::GetSkinAnime(const std::string & key)
{
    auto it = AnimatedImages.find(key);
    if (it == AnimatedImages.end()) return nullptr;
    it->second->AddRef();
    return it->second;
}

void RegisterScriptSkin(ExecutionManager *exm)
{
	auto engine = exm->GetScriptInterfaceUnsafe()->GetEngine();

    engine->RegisterObjectType(SU_IF_SKIN, 0, asOBJ_REF);
    engine->RegisterObjectBehaviour(SU_IF_SKIN, asBEHAVE_ADDREF, "void f()", asMETHOD(SkinHolder, AddRef), asCALL_THISCALL);
    engine->RegisterObjectBehaviour(SU_IF_SKIN, asBEHAVE_RELEASE, "void f()", asMETHOD(SkinHolder, Release), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SKIN, "void LoadImage(const string &in, const string &in)", asMETHOD(SkinHolder, LoadSkinImage), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SKIN, "void LoadFont(const string &in, const string &in)", asMETHOD(SkinHolder, LoadSkinFont), asCALL_THISCALL);
	engine->RegisterObjectMethod(SU_IF_SKIN, "void LoadSound(const string &in, const string &in)", asMETHOD(SkinHolder, LoadSkinSound), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SKIN, "void LoadAnime(const string &in, const string &in, int, int, int, int, int, double)", asMETHOD(SkinHolder, LoadSkinAnime), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SKIN, SU_IF_IMAGE "@ GetImage(const string &in)", asMETHOD(SkinHolder, GetSkinImage), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SKIN, SU_IF_FONT "@ GetFont(const string &in)", asMETHOD(SkinHolder, GetSkinFont), asCALL_THISCALL);
	engine->RegisterObjectMethod(SU_IF_SKIN, SU_IF_SOUND "@ GetSound(const string &in)", asMETHOD(SkinHolder, GetSkinSound), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SKIN, SU_IF_ANIMEIMAGE "@ GetAnime(const string &in)", asMETHOD(SkinHolder, GetSkinAnime), asCALL_THISCALL);

    engine->RegisterGlobalFunction(SU_IF_SKIN "@ GetSkin()", asFUNCTION(GetSkinObject), asCALL_CDECL);
}

//スキン専用
SkinHolder* GetSkinObject()
{
    auto ctx = asGetActiveContext();
    auto obj = (asIScriptObject*)ctx->GetThisPointer();
    if (!obj)
    {
        ScriptSceneWarnOutOf("Instance Method", ctx);
        return nullptr;
    }
    auto skin = obj->GetUserData(SU_UDTYPE_SKIN);
    if (!skin)
    {
        ScriptSceneWarnOutOf("Skin-Related Scene", ctx);
        return nullptr;
    }
    return (SkinHolder*)skin;
}