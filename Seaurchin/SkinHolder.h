#pragma once

#include "AngelScriptManager.h"
#include "ScriptResource.h"
#include "Misc.h"

class SkinHolder final
{
private:
    std::shared_ptr<AngelScript> ScriptInterface;
    std::string SkinName;
    boost::filesystem::path SkinRoot;
    std::unordered_map<std::string, SImage*> Images;
    std::unordered_map<std::string, SFont*> Fonts;
    //std::unordered_map<std::string, shared_ptr<Image>> Images;
    //std::unordered_map<std::string, shared_ptr<Image>> Images;

    bool IncludeScript(std::string include, std::string from, CScriptBuilder *builder);

public:
    SkinHolder(std::string name, std::shared_ptr<AngelScript> manager);
    ~SkinHolder();
    void AddRef();
    void Release();

    void Initialize();
    asIScriptObject* ExecuteSkinScript(std::string file);
    void LoadSkinImage(const std::string &key, const std::string &filename);
    void LoadSkinFont(const std::string &key, const std::string &filename);
    SImage* GetSkinImage(const std::string &key);
    SFont* GetSkinFont(const std::string &key);
};

SkinHolder* GetSkinObject();