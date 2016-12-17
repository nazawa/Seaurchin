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
    void CreateMusicCache();

public:
    MusicsManager(std::shared_ptr<Setting> setting);
    ~MusicsManager();

    void Initialize();
    void Reload(bool recreateCache);

};