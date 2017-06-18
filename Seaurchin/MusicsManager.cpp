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
    Applied = new MusicSelectionCursor(this);
    Applied->AddRef();
}

MusicsManager::~MusicsManager()
{}

void MusicsManager::Initialize()
{}

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

std::string MusicsManager::GetSelectedScorePath()
{
    path result = Setting::GetRootDirectory() / SU_MUSIC_DIR / ConvertUTF8ToShiftJis(Categories[Applied->CategoryIndex]->GetName());
    result /= Categories[Applied->CategoryIndex]->Musics[Applied->MusicIndex]->Scores[Applied->VariantIndex]->Path;
    return result.string();
}

MusicSelectionCursor * MusicsManager::CreateCursor()
{
    Applied->AddRef();
    return Applied;
}

void MusicsManager::CreateMusicCache()
{
    FlagMutex.lock();
    Loading = true;
    FlagMutex.unlock();

    path mlpath = Setting::GetRootDirectory() / SU_MUSIC_DIR;
    for (const auto& fdata : make_iterator_range(directory_iterator(mlpath), {})) {
        if (!is_directory(fdata)) continue;

        auto category = make_shared<CategoryInfo>(fdata);
        for (const auto& mdir : make_iterator_range(directory_iterator(fdata), {})) {
            if (!is_directory(mdir)) continue;
            for (const auto& file : make_iterator_range(directory_iterator(mdir), {})) {
                if (is_directory(file)) continue;
                if (file.path().extension() != ".sus") continue;     //‚±‚ê‘å•¶Žš‚Ç‚¤‚·‚ñ‚Ì
                Analyzer->Reset();
                Analyzer->LoadFromFile(file.path().string(), true);
                auto music = find_if(category->Musics.begin(), category->Musics.end(), [&](std::shared_ptr<MusicMetaInfo> info) {
                    return info->SongId == Analyzer->SharedMetaData.USongId;
                });
                if (music == category->Musics.end()) {
                    music = category->Musics.insert(category->Musics.begin(), make_shared<MusicMetaInfo>());
                    (*music)->SongId = Analyzer->SharedMetaData.USongId;
                    (*music)->Name = Analyzer->SharedMetaData.UTitle;
                    (*music)->Artist = Analyzer->SharedMetaData.UArtist;
                    (*music)->JacketPath = (mdir.path().filename() / ConvertUTF8ToShiftJis(Analyzer->SharedMetaData.UJacketFileName)).string();
                }
                auto score = make_shared<MusicScoreInfo>();
                score->Path = mdir.path().filename() / file.path().filename();
                score->WavePath = ConvertUTF8ToShiftJis(Analyzer->SharedMetaData.UWaveFileName);
                score->Designer = Analyzer->SharedMetaData.UDesigner;
                score->Difficulty = Analyzer->SharedMetaData.DifficultyType;
                score->Level = Analyzer->SharedMetaData.Level;
                (*music)->Scores.push_back(score);
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
    Name = ConvertShiftJisToUTF8(path.filename().string());
}

CategoryInfo::~CategoryInfo()
{}

void CategoryInfo::Reload(bool recreateCache)
{

}

//MusicSelectionCursor ------------------------------------------

std::shared_ptr<MusicMetaInfo> MusicSelectionCursor::GetMusicAt(int32_t relative)
{
    auto current = Manager->Categories[CategoryIndex];
    if (current->Musics.size() == 0) return nullptr;
    int32_t actual = relative + MusicIndex;
    while (actual < 0) actual += current->Musics.size();
    return current->Musics[actual % current->Musics.size()];
}

std::shared_ptr<MusicScoreInfo> MusicSelectionCursor::GetScoreVariantAt(int32_t relative)
{
    auto music = GetMusicAt(relative);
    if (!music) return nullptr;
    auto variant = music->Scores[max((unsigned int)VariantIndex, music->Scores.size() - 1)];
    return variant;
}

MusicSelectionCursor::MusicSelectionCursor(MusicsManager *manager)
{
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

string MusicSelectionCursor::GetCategoryName(int32_t relativeIndex)
{
    if (Manager->Categories.size() == 0) return "Unavailable";
    int32_t actual = relativeIndex + CategoryIndex;
    while (actual < 0) actual += Manager->Categories.size();
    return Manager->Categories[actual % Manager->Categories.size()]->GetName();
}

string MusicSelectionCursor::GetMusicName(int32_t relativeIndex)
{
    auto music = GetMusicAt(relativeIndex);
    return music ? music->Name : "Unavailable!";
}

string MusicSelectionCursor::GetArtistName(int32_t relativeIndex)
{
    auto music = GetMusicAt(relativeIndex);
    return music ? music->Artist : "Unavailable!";
}

string MusicSelectionCursor::GetMusicJacketFileName(int32_t relativeIndex)
{
    auto music = GetMusicAt(relativeIndex);
    auto current = Manager->Categories[CategoryIndex];
    if (!music) return "";
    if (music->JacketPath == "") return "";
    auto result = (Setting::GetRootDirectory() / SU_MUSIC_DIR / ConvertUTF8ToShiftJis(current->GetName()) / music->JacketPath).string();
    return ConvertShiftJisToUTF8(result);
}

int MusicSelectionCursor::GetDifficulty(int32_t relativeIndex)
{
    auto variant = GetScoreVariantAt(relativeIndex);
    return variant->Difficulty;
}

int MusicSelectionCursor::GetLevel(int32_t relativeIndex)
{
    auto variant = GetScoreVariantAt(relativeIndex);
    return variant->Level;
}

std::string MusicSelectionCursor::GetDesignerName(int32_t relativeIndex)
{
    auto variant = GetScoreVariantAt(relativeIndex);
    return variant->Designer;
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
            auto nm = GetMusicAt(MusicIndex);
            VariantIndex = max((unsigned int)VariantIndex, nm->Scores.size() - 1);
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
            auto nm = GetMusicAt(MusicIndex);
            VariantIndex = max((unsigned int)VariantIndex, nm->Scores.size() - 1);
        }
        default:
            return 0;
    }
    return 0;
}

int MusicSelectionCursor::NextVariant()
{
    switch (State) {
        case 0:
            return -1;
        case 1: {
            auto music = GetMusicAt(MusicIndex);
            if (!music) return -1;
            VariantIndex = (VariantIndex + 1) % music->Scores.size();
        }
        default:
            return 0;
    }
    return 0;
}

int MusicSelectionCursor::PreviousVariant()
{
    switch (State) {
        case 0:
            return -1;
        case 1: {
            auto music = GetMusicAt(MusicIndex);
            if (!music) return -1;
            VariantIndex = (VariantIndex + music->Scores.size() - 1) % music->Scores.size();
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
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "string GetArtistName(int)", asMETHOD(MusicSelectionCursor, GetArtistName), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "string GetMusicJacketFileName(int)", asMETHOD(MusicSelectionCursor, GetMusicJacketFileName), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "int GetDifficulty(int)", asMETHOD(MusicSelectionCursor, GetArtistName), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "int GetLevel(int)", asMETHOD(MusicSelectionCursor, GetArtistName), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "string GetDesignerName(int)", asMETHOD(MusicSelectionCursor, GetArtistName), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "int Next()", asMETHOD(MusicSelectionCursor, Next), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "int Previous()", asMETHOD(MusicSelectionCursor, Previous), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "int NextVariant()", asMETHOD(MusicSelectionCursor, NextVariant), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "int PreviousVariant()", asMETHOD(MusicSelectionCursor, PreviousVariant), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "int Enter()", asMETHOD(MusicSelectionCursor, Enter), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, "bool Exit()", asMETHOD(MusicSelectionCursor, Exit), asCALL_THISCALL);
}
