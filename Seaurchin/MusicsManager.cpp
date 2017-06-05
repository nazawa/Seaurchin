#include "MusicsManager.h"

#include "Config.h"
#include "Misc.h"
#include "Debug.h"

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace boost::xpressive;

//path sepath = Setting::GetRootDirectory() / SU_DATA_DIR / SU_SKIN_DIR;

MusicsManager::MusicsManager(std::shared_ptr<Setting> setting)
{
    SharedSetting = setting;
    Analyzer = make_unique<SusAnalyzer>(192);
}

MusicsManager::~MusicsManager()
{
}

void MusicsManager::Initialize()
{
}

void MusicsManager::Reload(bool recreateCache)
{   /*
    if (recreateCache) {
        thread loadthread([this] { CreateMusicCache(); });
        loadthread.detach();
    }
    */
    CreateMusicCache();
}

bool MusicsManager::IsReloading()
{
    FlagMutex.lock();
    bool state = Loading;
    FlagMutex.unlock();
    return state;
}

MusicSelectionCursor * MusicsManager::CreateCursor()
{
    auto cursor = new MusicSelectionCursor(this);
    cursor->AddRef();
    return cursor;
}

void MusicsManager::CreateMusicCache()
{
    FlagMutex.lock();
    Loading = true;
    FlagMutex.unlock();

    path mlpath = Setting::GetRootDirectory() / SU_MUSIC_DIR;
    for (const auto& fdata : make_iterator_range(directory_iterator(mlpath), {}))
    {
        if (!is_directory(fdata)) continue;
        auto category = make_shared<CategoryInfo>(mlpath / fdata);
        for (const auto& mdir : make_iterator_range(directory_iterator(fdata), {}))
        {
            if (!is_directory(mdir)) continue;

            for (const auto& file : make_iterator_range(directory_iterator(mdir), {}))
            {
                if (is_directory(file)) continue;
                if (file.path().extension() != ".sus") continue;     //‚±‚ê‘å•¶Žš‚Ç‚¤‚·‚ñ‚Ì
                Analyzer->Reset();
                Analyzer->LoadFromFile(file.path().string(), true);
                auto info = make_shared<MusicMetaInfo>();
                info->Path = file;
                info->SongId = Analyzer->SharedMetaData.USongId;
                info->Name = Analyzer->SharedMetaData.UTitle;
                info->DifficultyName = Analyzer->SharedMetaData.USubTitle;
                info->Artist = Analyzer->SharedMetaData.UArtist;
                info->Designer = Analyzer->SharedMetaData.UDesigner;
                category->Musics.push_back(info);
            }
        }
        Categories.push_back(category);
    }

    FlagMutex.lock();
    Loading = false;
    FlagMutex.unlock();
}

// CategoryInfo ---------------------------

CategoryInfo::CategoryInfo(boost::filesystem::path path)
{
    Path = path;
    Name = path.filename().string();
}

CategoryInfo::~CategoryInfo()
{
}

void CategoryInfo::Reload(bool recreateCache)
{

}

//MusicSelectionCursor ------------------------------------------
MusicSelectionCursor::MusicSelectionCursor(MusicsManager *manager) {
    Manager = manager;
    CategoryIndex = 0;
    MusicIndex = -1;
    VariantIndex = -1;
}

std::string MusicSelectionCursor::GetPrimaryString(int32_t relativeIndex)
{
    switch (State) {
    case 0:
        return GetCategoryName(relativeIndex);
    case 1:
        return GetMusicName(relativeIndex);
    default:
        return "";
    }
    
}

std::string MusicSelectionCursor::GetCategoryName(int32_t relativeIndex)
{
    if (Manager->Categories.size() == 0) return "Unavailable";
    int32_t actual = relativeIndex + CategoryIndex;
    while (actual < 0) actual += Manager->Categories.size();
    return Manager->Categories[actual % Manager->Categories.size()]->GetName();
}

std::string MusicSelectionCursor::GetMusicName(int32_t relativeIndex)
{
    auto current = Manager->Categories[CategoryIndex];
    if (current->Musics.size() == 0) return "Unavailable!";
    int32_t actual = relativeIndex + MusicIndex;
    while (actual < 0) actual += current->Musics.size();
    return current->Musics[actual % current->Musics.size()]->Name;
}

int MusicSelectionCursor::Enter()
{
    switch (State) {
    case 0:
        if (Manager->Categories.size() == 0) return 0;
        State = 1;
        MusicIndex = 0;
        VariantIndex = 0;
        return 1;
    case 1: {
        //‘I‹ÈI—¹
        auto current = Manager->Categories[CategoryIndex];
        Manager->Analyzer->LoadFromFile(current->Musics[MusicIndex]->Path.string(), false);
        return 2;
    }
    default:
        return 0;
    }
}

bool MusicSelectionCursor::Exit()
{
    switch (State) {
    case 0:
        return 0;
    case 1:
        State = 0;
        return 1;
    default:
        return 0;
    }
}

bool MusicSelectionCursor::Start()
{
    return false;
}

int MusicSelectionCursor::Next()
{
    switch (State) {
    case 0:
        if (Manager->Categories.size() == 0) return -1;
        CategoryIndex = (CategoryIndex + 1) % Manager->Categories.size();
        return 1;
    case 1: {
        auto current = Manager->Categories[CategoryIndex];
        if (current->Musics.size() == 0) return -1;
        MusicIndex = (MusicIndex + 1) % current->Musics.size();
    }
    default:
        return 0;
    }
    return 0;
}

int MusicSelectionCursor::Previous()
{
    switch (State) {
    case 0:
        if (Manager->Categories.size() == 0) return -1;
        CategoryIndex = (CategoryIndex + Manager->Categories.size() - 1) % Manager->Categories.size();
        return 1;
    case 1: {
        auto current = Manager->Categories[CategoryIndex];
        if (current->Musics.size() == 0) return -1;
        MusicIndex = (MusicIndex + current->Musics.size() - 1) % current->Musics.size();
    }
    default:
        return 0;
    }
    return 0;
}

void MusicSelectionCursor::RegisterScriptInterface(asIScriptEngine *engine)
{
    engine->RegisterObjectType(SU_IF_MSCURSOR, 0, asOBJ_REF);
    engine->RegisterObjectBehaviour(SU_IF_MSCURSOR, asBEHAVE_ADDREF, "void f()", asMETHOD(MusicSelectionCursor, AddRef), asCALL_THISCALL);
    engine->RegisterObjectBehaviour(SU_IF_MSCURSOR, asBEHAVE_RELEASE, "void f()", asMETHOD(MusicSelectionCursor, Release), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "string GetPrimaryString(int)", asMETHOD(MusicSelectionCursor, GetPrimaryString), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "string GetCategoryName(int)", asMETHOD(MusicSelectionCursor, GetCategoryName), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "string GetMusicName(int)", asMETHOD(MusicSelectionCursor, GetMusicName), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "int Next()", asMETHOD(MusicSelectionCursor, Next), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "int Previous()", asMETHOD(MusicSelectionCursor, Previous), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "int Enter()", asMETHOD(MusicSelectionCursor, Enter), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "bool Exit()", asMETHOD(MusicSelectionCursor, Exit), asCALL_THISCALL);
}
