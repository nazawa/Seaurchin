#include "MusicsManager.h"
#include "ExecutionManager.h"
#include "Config.h"
#include "Misc.h"
#include "Debug.h"

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace boost::xpressive;

//path sepath = Setting::GetRootDirectory() / SU_DATA_DIR / SU_SKIN_DIR;

MusicsManager::MusicsManager(ExecutionManager *exm)
{
    Manager = exm;
    SharedSetting = Manager->GetSettingInstanceSafe();
    Analyzer = make_unique<SusAnalyzer>(192);
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
    int ci = Manager->GetData<int>("Selected:Category");
    int mi = Manager->GetData<int>("Selected:Music");
    int vi = Manager->GetData<int>("Selected:Variant");
    path result = Setting::GetRootDirectory() / SU_MUSIC_DIR / ConvertUTF8ToShiftJis(Categories[ci]->GetName());
    result /= Categories[ci]->Musics[mi]->Scores[vi]->Path;
    return result.string();
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

MusicSelectionCursor *MusicsManager::CreateMusicSelectionCursor()
{
    auto result = new MusicSelectionCursor(this);
    result->AddRef();
    return result;
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
    auto variant = music->Scores[min((unsigned int)VariantIndex, music->Scores.size() - 1)];
    return variant;
}

MusicSelectionCursor::MusicSelectionCursor(MusicsManager *manager)
{
    Manager = manager;
    CategoryIndex = 0;
    MusicIndex = -1;
    VariantIndex = -1;
    State = MusicSelectionState::Category;
}

std::string MusicSelectionCursor::GetPrimaryString(int32_t relativeIndex)
{
    switch (State) {
        case MusicSelectionState::Category:
            return GetCategoryName(relativeIndex);
        case MusicSelectionState::Music:
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

MusicSelectionState MusicSelectionCursor::Enter()
{
    switch (State) {
        case MusicSelectionState::Category:
            if (Manager->Categories.size() == 0) return MusicSelectionState::OutOfFunction;
            State = MusicSelectionState::Music;
            MusicIndex = 0;
            VariantIndex = 0;
            return State;
        case MusicSelectionState::Music:
            //‘I‹ÈI—¹
            Manager->Manager->SetData<int>("Selected:Category", CategoryIndex);
            Manager->Manager->SetData<int>("Selected:Music", MusicIndex);
            Manager->Manager->SetData<int>("Selected:Variant", VariantIndex);
            return MusicSelectionState::Confirmed;
        default:
            return MusicSelectionState::Success;
    }
}

MusicSelectionState MusicSelectionCursor::Exit()
{
    switch (State) {
        case MusicSelectionState::Category:
            State = MusicSelectionState::OutOfFunction;
            break;
        case MusicSelectionState::Music:
            State = MusicSelectionState::Category;
            break;
    }
    return State;
}

MusicSelectionState MusicSelectionCursor::Start()
{
    return MusicSelectionState::Success;
}

MusicSelectionState MusicSelectionCursor::Next()
{
    switch (State) {
        case MusicSelectionState::Category:
            if (Manager->Categories.size() == 0) return MusicSelectionState::Error;
            CategoryIndex = (CategoryIndex + 1) % Manager->Categories.size();
            break;
        case MusicSelectionState::Music: {
            auto current = Manager->Categories[CategoryIndex];
            if (current->Musics.size() == 0) return MusicSelectionState::Error;
            MusicIndex = (MusicIndex + 1) % current->Musics.size();
            auto nm = GetMusicAt(MusicIndex);
            VariantIndex = min((unsigned int)VariantIndex, nm->Scores.size() - 1);
            break;
        }
    }
    return MusicSelectionState::Success;
}

MusicSelectionState MusicSelectionCursor::Previous()
{
    switch (State) {
        case MusicSelectionState::Category:
            if (Manager->Categories.size() == 0) return MusicSelectionState::Error;
            CategoryIndex = (CategoryIndex + Manager->Categories.size() - 1) % Manager->Categories.size();
            break;
        case MusicSelectionState::Music: {
            auto current = Manager->Categories[CategoryIndex];
            if (current->Musics.size() == 0) return MusicSelectionState::Error;
            MusicIndex = (MusicIndex + current->Musics.size() - 1) % current->Musics.size();
            auto nm = GetMusicAt(MusicIndex);
            VariantIndex = min((unsigned int)VariantIndex, nm->Scores.size() - 1);
            break;
        }
    }
    return MusicSelectionState::Success;
}

MusicSelectionState MusicSelectionCursor::NextVariant()
{
    switch (State) {
        case MusicSelectionState::Category:
            return MusicSelectionState::Error;
        case MusicSelectionState::Music: {
            auto music = GetMusicAt(MusicIndex);
            if (!music) return MusicSelectionState::Error;
            VariantIndex = (VariantIndex + 1) % music->Scores.size();
            break;
        }
    }
    return MusicSelectionState::Success;
}

MusicSelectionState MusicSelectionCursor::PreviousVariant()
{
    switch (State) {
        case MusicSelectionState::Category:
            return MusicSelectionState::Error;
        case MusicSelectionState::Music: {
            auto music = GetMusicAt(MusicIndex);
            if (!music) return MusicSelectionState::Error;
            VariantIndex = (VariantIndex + music->Scores.size() - 1) % music->Scores.size();
        }
    }
    return MusicSelectionState::Success;
}

MusicSelectionState MusicSelectionCursor::GetState()
{
    return State;
}

void MusicSelectionCursor::RegisterScriptInterface(asIScriptEngine *engine)
{
    engine->RegisterEnum(SU_IF_MSCSTATE);
    engine->RegisterEnumValue(SU_IF_MSCSTATE, "OutOfFunction", (int)MusicSelectionState::OutOfFunction);
    engine->RegisterEnumValue(SU_IF_MSCSTATE, "Category", (int)MusicSelectionState::Category);
    engine->RegisterEnumValue(SU_IF_MSCSTATE, "Music", (int)MusicSelectionState::Music);
    engine->RegisterEnumValue(SU_IF_MSCSTATE, "Confirmed", (int)MusicSelectionState::Confirmed);
    engine->RegisterEnumValue(SU_IF_MSCSTATE, "Error", (int)MusicSelectionState::Error);
    engine->RegisterEnumValue(SU_IF_MSCSTATE, "Success", (int)MusicSelectionState::Success);

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
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, SU_IF_MSCSTATE " Next()", asMETHOD(MusicSelectionCursor, Next), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, SU_IF_MSCSTATE " Previous()", asMETHOD(MusicSelectionCursor, Previous), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, SU_IF_MSCSTATE " NextVariant()", asMETHOD(MusicSelectionCursor, NextVariant), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, SU_IF_MSCSTATE " PreviousVariant()", asMETHOD(MusicSelectionCursor, PreviousVariant), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, SU_IF_MSCSTATE " Enter()", asMETHOD(MusicSelectionCursor, Enter), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, SU_IF_MSCSTATE " Exit()", asMETHOD(MusicSelectionCursor, Exit), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_MSCURSOR, SU_IF_MSCSTATE " GetState()", asMETHOD(MusicSelectionCursor, GetState), asCALL_THISCALL);
}
