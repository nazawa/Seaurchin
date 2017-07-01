#pragma once

#define SU_SETTING_GENERAL "General"
#define SU_SETTING_SKIN "Skin"

class Setting final
{
private:
    static std::string RootDirectory;
    std::string file;
    boost::property_tree::ptree SettingTree;

public:
    Setting();
    Setting(HMODULE hModule);
    void Load(std::string filename);
    inline void Reload() { Load(file); }
    void Save() const;
    static const std::string GetRootDirectory();

    template<typename T>
    T ReadValue(std::string group, std::string key, T defValue)
    {
        boost::optional<T> v = SettingTree.get_optional<T>(group + "." + key);
        if (v) {
            return v.get();
        } else {
            WriteValue(group, key, defValue);
            return defValue;
        }
    }

    template<typename T>
    void WriteValue(std::string group, std::string key, T value)
    {
        SettingTree.put<T>(group + "." + key, value);
    }
};