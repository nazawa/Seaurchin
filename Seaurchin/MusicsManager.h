#pragma once

#include "SusAnalyzer.h"
#include "Setting.h"

#define SU_IF_MSCURSOR "MusicCursor"
#define SU_IF_MSCSTATE "CursorState"

struct MusicScoreInfo final {
    uint16_t Difficulty;
    uint16_t Level;
    std::string DifficultyName;
    std::string Designer;
    boost::filesystem::path Path;
    boost::filesystem::path WavePath;
};

struct MusicMetaInfo final {
    std::string SongId;
    std::string Name;
    std::string Artist;
    std::string JacketPath;
    std::vector<std::shared_ptr<MusicScoreInfo>> Scores;
};


class CategoryInfo final {
private:

    std::string Name;
    boost::filesystem::path Path;

public:
    std::vector<std::shared_ptr<MusicMetaInfo>> Musics;

    CategoryInfo(boost::filesystem::path path);
    ~CategoryInfo();

    std::string GetName() { return Name; }
    void Reload(bool recreateCache);
};

//musicにsはつかないって？知るかバカ
class MusicSelectionCursor;
enum class MusicSelectionState {
    OutOfFunction = 0,
    Category,
    Music,
    Variant,
    Confirmed,

    Error,
    Success,
};

class ExecutionManager;
class MusicsManager final {
    friend class MusicSelectionCursor;
private:
    std::shared_ptr<Setting> SharedSetting;
    ExecutionManager *Manager;

    bool Loading = false;
    std::mutex FlagMutex;
    std::unique_ptr<SusAnalyzer> Analyzer;
    std::vector<std::shared_ptr<CategoryInfo>> Categories;
    void CreateMusicCache();

public:
    MusicsManager(ExecutionManager *exm);
    ~MusicsManager();

    void Initialize();
    void Reload(bool recreateCache);
    bool IsReloading();
    std::string GetSelectedScorePath();

    MusicSelectionCursor *CreateMusicSelectionCursor();
    const std::vector<std::shared_ptr<CategoryInfo>> &GetCategories() { return Categories; }
};

class MusicSelectionCursor final {
    friend class MusicsManager;
private:
    int refcount = 0;

    MusicsManager *Manager;
    int32_t CategoryIndex;
    int32_t MusicIndex;
    uint16_t VariantIndex;
    MusicSelectionState State;

    std::shared_ptr<MusicMetaInfo> GetMusicAt(int32_t relative);
    std::shared_ptr<MusicScoreInfo> GetScoreVariantAt(int32_t relative);

public:
    MusicSelectionCursor(MusicsManager *manager);
    void AddRef() { refcount++; }
    void Release() { if (--refcount == 0) delete this; }

    std::string GetPrimaryString(int32_t relativeIndex);
    std::string GetCategoryName(int32_t relativeIndex);
    std::string GetMusicName(int32_t relativeIndex);
    std::string GetArtistName(int32_t relativeIndex);
    std::string GetMusicJacketFileName(int32_t relativeIndex);
    int GetDifficulty(int32_t relativeIndex);
    int GetLevel(int32_t relativeIndex);
    std::string GetDesignerName(int32_t relativeIndex);

    MusicSelectionState Enter();
    MusicSelectionState Exit();
    MusicSelectionState Start();
    MusicSelectionState Next();
    MusicSelectionState Previous();
    MusicSelectionState NextVariant();
    MusicSelectionState PreviousVariant();
    MusicSelectionState GetState();

    static void RegisterScriptInterface(asIScriptEngine *engine);
};

struct MusicRawData final {
    std::string SongId;
    std::string Name;
    std::string Artist;
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> Scores;
};