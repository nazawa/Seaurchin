#pragma once

#include "SusAnalyzer.h"
#include "Setting.h"

#define SU_IF_MSCURSOR "MusicCursor"

struct MusicMetaInfo final {
    boost::filesystem::path Path;
    std::string SongId;
    std::string Name;
    std::string DifficultyName;
    std::string Artist;
    std::string Designer;
    uint32_t Level;
    uint32_t Difficulty;
};

class CategoryInfo final
{
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

class MusicsManager final
{
    friend class MusicSelectionCursor;
private:
    std::shared_ptr<Setting> SharedSetting;

    bool Loading = false;
    std::mutex FlagMutex;
    std::unique_ptr<SusAnalyzer> Analyzer;
    std::vector<std::shared_ptr<CategoryInfo>> Categories;

    void CreateMusicCache();

public:
    MusicsManager(std::shared_ptr<Setting> setting);
    ~MusicsManager();

    void Initialize();
    void Reload(bool recreateCache);
    bool IsReloading();
    
    MusicSelectionCursor *CreateCursor();
};

class MusicSelectionCursor final {
    friend class MusicsManager;
private:
    int refcount = 0;

    MusicsManager *Manager;
    int32_t CategoryIndex;
    int32_t MusicIndex;
    uint16_t VariantIndex;
    uint16_t State;

public:
    MusicSelectionCursor(MusicsManager *manager);
    void AddRef() { refcount++; }
    void Release() { if (--refcount == 0) delete this; }

    std::string GetPrimaryString(int32_t relativeIndex);
    std::string GetCategoryName(int32_t relativeIndex);
    std::string GetMusicName(int32_t relativeIndex);

    int Enter();
    bool Exit();
    bool Start();
    int Next();
    int Previous();

    static void RegisterScriptInterface(asIScriptEngine *engine);
};
