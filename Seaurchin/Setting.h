#pragma once

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
};