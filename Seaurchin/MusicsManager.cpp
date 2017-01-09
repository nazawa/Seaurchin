#include "MusicsManager.h"

#include "Config.h"
#include "Debug.h"

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace boost::xpressive;

path sepath = Setting::GetRootDirectory() / SU_DATA_DIR / SU_SKIN_DIR;

MusicsManager::MusicsManager(std::shared_ptr<Setting> setting)
{
    SharedSetting = setting;
}

MusicsManager::~MusicsManager()
{
}

void MusicsManager::Initialize()
{
}

void MusicsManager::Reload(bool recreateCache)
{
    if (recreateCache) {
        thread loadthread([this] { CreateMusicCache(); });
        loadthread.detach();
    }
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
    for (const auto& fdata : make_iterator_range(directory_iterator(sepath), {}))
    {
        if (!is_directory(fdata)) continue;
        WriteDebugConsole("Category Found: ");
        WriteDebugConsole(fdata.path().filename().string().c_str());
        WriteDebugConsole("\n");
        for (const auto& mdir : make_iterator_range(directory_iterator(fdata), {}))
        {
            if (!is_directory(mdir)) continue;
            WriteDebugConsole("Music Dir Found: ");
            WriteDebugConsole(mdir.path().filename().string().c_str());
            WriteDebugConsole("\n");
        }
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
