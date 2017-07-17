#include "Setting.h"
#include "Config.h"
#include "Debug.h"
#include "Misc.h"

using namespace std;
using namespace boost::property_tree;
using namespace boost::filesystem;
namespace ba = boost::algorithm;

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

SettingItem::SettingItem(shared_ptr<Setting> setting, const string &group, const string &key) : SettingInstance(setting), SettingGroup(group), SettingKey(key)
{

}

// NumberSettingItem -------------------------------------------------------------------

function<string(NumberSettingItem*)> NumberSettingItem::DefaultFormatter = [](NumberSettingItem *item) {

    ostringstream ss;
    ss << setprecision(item->GetFloatDigits()) << item->GetValue();
    return ss.str();
};

NumberSettingItem::NumberSettingItem(shared_ptr<Setting> setting, const string &group, const string &key) : SettingItem(setting, group, key)
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
    Type = digits > 0 ? SettingType::FloatSetting : SettingType::IntegerSetting;
}

BooleanSettingItem::BooleanSettingItem(shared_ptr<Setting> setting, const string &group, const string &key) : SettingItem(setting, group, key)
{
    Type = SettingType::BooleanSetting;
}

string BooleanSettingItem::GetItemString()
{
    return Value ? TrueText : FalseText;
}

void BooleanSettingItem::MoveNext()
{
    Value = !Value;
}

void BooleanSettingItem::MovePrevious()
{
    Value = !Value;
}

void BooleanSettingItem::SaveValue()
{
    SettingInstance->WriteValue<bool>(SettingGroup, SettingKey, Value);
}

void BooleanSettingItem::RetrieveValue()
{
    Value = SettingInstance->ReadValue<bool>(SettingGroup, SettingKey, false);
}

void BooleanSettingItem::SetText(const string &t, const string &f)
{
    TrueText = t;
    FalseText = f;
}

SettingItemManager::SettingItemManager(std::shared_ptr<Setting> setting) : ReferredSetting(setting)
{

}

void SettingItemManager::AddSettingByString(const string &proc)
{
    //B:<Group>:<Key>:Description:YesText:NoText:Default
    //FI:<Group>:<Key>:Description:Min:Max:Step:Default
    auto str = proc;
    vector<string> params;
    ba::erase_all(str, " ");
    ba::split(params, str, boost::is_any_of(":"));
    if (params.size() < 4) return;
    if (params[0] == "B") {
        ReferredSetting->ReadValue(params[1], params[2], ConvertBoolean(params[6]));
        auto si = make_shared<BooleanSettingItem>(ReferredSetting, params[1], params[2]);
        si->SetDescription(params[3]);
        si->SetText(params[4], params[5]);
        si->RetrieveValue();
        Items[params[1] + "." + params[2]] = si;
    } else if (params[0] == "I") {
        ReferredSetting->ReadValue(params[1], params[2], ConvertInteger(params[7]));
        auto si = make_shared<NumberSettingItem>(ReferredSetting, params[1], params[2]);
        si->SetDescription(params[3]);
        si->SetFloatDigits(0);
        si->SetRange(ConvertInteger(params[4]), ConvertInteger(params[5]));
        si->SetStep(ConvertInteger(params[6]));
        si->RetrieveValue();
        Items[params[1] + "." + params[2]] = si;
    } else if (params[0] == "F") {
        ReferredSetting->ReadValue(params[1], params[2], ConvertFloat(params[7]));
        auto si = make_shared<NumberSettingItem>(ReferredSetting, params[1], params[2]);
        si->SetDescription(params[3]);
        si->SetFloatDigits(5);
        si->SetRange(ConvertFloat(params[4]), ConvertFloat(params[5]));
        si->SetStep(ConvertFloat(params[6]));
        si->RetrieveValue();
        Items[params[1] + "." + params[2]] = si;
    }
}

std::shared_ptr<SettingItem> SettingItemManager::GetSettingItem(const string &group, const string &key)
{
    return Items[group + "." + key];
}

void SettingItemManager::RetrieveAllValues()
{
    for (auto &si : Items) si.second->RetrieveValue();
}

void SettingItemManager::SaveAllValues()
{
    for (auto &si : Items) si.second->SaveValue();
}

