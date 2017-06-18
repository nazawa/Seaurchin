#pragma once

#include "AngelScriptManager.h"
#include "SoundManager.h"
#include "ScriptResource.h"
#include "Misc.h"

#define SU_IF_SKIN "Skin"

class SkinHolder final {
private:
    std::shared_ptr<AngelScript> ScriptInterface;
    std::shared_ptr<SoundManager> SoundInterface;
    std::string SkinName;
    boost::filesystem::path SkinRoot;
    std::unordered_map<std::string, SImage*> Images;
    std::unordered_map<std::string, SFont*> Fonts;
    std::unordered_map<std::string, SSound*> Sounds;
    std::unordered_map<std::string, SAnimatedImage*> AnimatedImages;
    //std::unordered_map<std::string, shared_ptr<Image>> Images;

    bool IncludeScript(std::string include, std::string from, CScriptBuilder *builder);

public:
    SkinHolder(std::string name, std::shared_ptr<AngelScript> script, std::shared_ptr<SoundManager> sound);
    ~SkinHolder();
    void AddRef();
    void Release();

    void Initialize();
    asIScriptObject* ExecuteSkinScript(std::string file);
    void LoadSkinImage(const std::string &key, const std::string &filename);
    void LoadSkinFont(const std::string &key, const std::string &filename);
    void LoadSkinSound(const std::string &key, const std::string &filename);
    void LoadSkinAnime(const std::string &key, const std::string &filename, int x, int y, int w, int h, int c, double time);
    SImage* GetSkinImage(const std::string &key);
    SFont* GetSkinFont(const std::string &key);
    SSound* GetSkinSound(const std::string &key);
    SAnimatedImage* GetSkinAnime(const std::string &key);
};

class ExecutionManager;
void RegisterScriptSkin(ExecutionManager *exm);
SkinHolder* GetSkinObject();