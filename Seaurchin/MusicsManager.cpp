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
                Analyzer->LoadFromFile(file.path().string());
                auto info = make_shared<MusicMetaInfo>();
                info->Path = mdir / file;
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
