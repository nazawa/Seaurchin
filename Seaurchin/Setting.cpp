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

// SettingItem -----------------------------------------------------------------

SettingItem::SettingItem(shared_ptr<Setting> setting, string group, string key) : SettingInstance(setting), SettingGroup(group), SettingKey(key)
{

}

// NumberSettingItem -------------------------------------------------------------------

function<string(NumberSettingItem*)> NumberSettingItem::DefaultFormatter = [](NumberSettingItem *item) {

    return "";
};

NumberSettingItem::NumberSettingItem(shared_ptr<Setting> setting, string group, string key) : SettingItem(setting, group, key)
{
    Formatter = DefaultFormatter;
    Type = SettingType::IntegerSetting;
}

std::string NumberSettingItem::GetItemString()
{
    return Formatter(this);
}

void NumberSettingItem::MoveNext()
{
    Value += Step;
    if (Value > MaxValue) Value = MaxValue;
}

void NumberSettingItem::MovePrevious()
{
    Value -= Step;
    if (Value < MinValue) Value = MinValue;
}

void NumberSettingItem::SaveValue()
{
    SettingInstance->WriteValue<double>(SettingGroup, SettingKey, Value);
}

void NumberSettingItem::RetrieveValue()
{
    Value = SettingInstance->ReadValue<double>(SettingGroup, SettingKey, 0);
}

void NumberSettingItem::SetStep(double step)
{
    Step = fabs(step);
}

void NumberSettingItem::SetRange(double min, double max)
{
    if (max < min) return;
    MinValue = min;
    MaxValue = max;
}

void NumberSettingItem::SetFloatDigits(int digits)
{
    if (digits < 0) return;
    FloatDigits = digits;
}



