#pragma once

#include "Setting.h"

class ScoreData final
{

};

class MusicInfo final
{

};

class CategoryInfo final
{
private:
    
    std::string Name;
    boost::filesystem::path Path;


public:
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
    std::vector<std::shared_ptr<CategoryInfo>> Categories;

    void CreateMusicCache();

public:
    MusicsManager(std::shared_ptr<Setting> setting);
    ~MusicsManager();

    void Initialize();
    void Reload(bool recreateCache);
    bool IsReloading();

};