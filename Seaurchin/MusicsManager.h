#pragma once

#include "SusAnalyzer.h"
#include "Setting.h"

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

    void Reload(bool recreateCache);
};

//musicにsはつかないって？知るかバカ
class MusicsManager final
{
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

};