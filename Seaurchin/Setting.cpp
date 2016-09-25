#include "Setting.h"
#include "Config.h"
#include "Debug.h"

using namespace std;
using namespace boost::property_tree;
using namespace boost::filesystem;

std::string Setting::RootDirectory = "";

Setting::Setting(HMODULE hModule)
{
    if (RootDirectory != "") return;
    TCHAR directory[MAX_PATH];
    GetModuleFileName(hModule, directory, MAX_PATH);
    PathRemoveFileSpec(directory);
    RootDirectory = directory;
}

Setting::Setting()
{

}

void Setting::Load(string filename)
{
    file = filename;
    if (!exists(RootDirectory / file)) Save();
    read_json((RootDirectory / file).string(), SettingTree);
    WriteDebugConsole("Setting File Loaded.\n");
}

const std::string Setting::GetRootDirectory()
{
    return RootDirectory;
}

void Setting::Save() const
{
    write_json((RootDirectory / file).string(), SettingTree);
    WriteDebugConsole("Setting File Saved.\n");
}
