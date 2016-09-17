#include "Setting.h"
#include "Debug.h"

using namespace std;
using namespace boost::property_tree;

string RootDirectory;
ptree SettingTree;

void InitializeSetting(HMODULE hModule)
{
    TCHAR directory[MAX_PATH];
    GetModuleFileName(hModule, directory, MAX_PATH);
    PathRemoveFileSpec(directory);
    RootDirectory = directory;
}

void SettingLoadSetting()
{
    if (!PathFileExists(ROOT_FILE(RootDirectory, SU_SETTING_FILE).c_str())) SettingSaveSetting();
    read_json(ROOT_FILE(RootDirectory, SU_SETTING_FILE), SettingTree);
    WriteDebugConsole("Setting File Loaded.\n");
}

void SettingSaveSetting()
{
    write_json(ROOT_FILE(RootDirectory, SU_SETTING_FILE), SettingTree);
    WriteDebugConsole("Setting File Saved.\n");
}

int SettingReadIntegerValue(std::string group, std::string key, int default)
{
    auto value = SettingTree.get_optional<int>(group + "." + key);
    return value ? value.get() :  (SettingTree.put(group + "." + key, default), default);
}

double SettingReadFloatValue(std::string group, std::string key, double default)
{
    auto value = SettingTree.get_optional<double>(group + "." + key);
    return value ? value.get() : (SettingTree.put(group + "." + key, default), default);
}

std::string SettingReadStringValue(std::string group, std::string key, std::string default)
{
    auto value = SettingTree.get_optional<string>(group + "." + key);
    return value ? value.get() : (SettingTree.put(group + "." + key, default), default);
}

void SettingWriteIntegerValue(std::string group, std::string key, int value)
{
    SettingTree.put(group + "." + key, value);
}

void SettingWriteFloatValue(std::string group, std::string key, double value)
{
    SettingTree.put(group + "." + key, value);
}

void SettingWriteStringValue(std::string group, std::string key, std::string value)
{
    SettingTree.put(group + "." + key, value);
}
