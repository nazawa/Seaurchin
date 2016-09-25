#pragma once

#include "Setting.h"
#include "AngelScriptManager.h"
#include "DxLibResouce.h"
#include "Scene.h"
#include "ScriptScene.h"

class ExecutionManager;

class SkinHolder final
{
private:
    ExecutionManager *Manager;
    std::string SkinName;
    boost::filesystem::path SkinRoot;
    std::unordered_map<std::string, std::shared_ptr<Image>> Images;
    std::unordered_map<std::string, std::shared_ptr<Font>> Fonts;
    //std::unordered_map<std::string, shared_ptr<Image>> Images;
    //std::unordered_map<std::string, shared_ptr<Image>> Images;

    bool IncludeScript(std::string include, std::string from, CScriptBuilder *builder);

public:
    SkinHolder(std::string name, ExecutionManager *manager);
    ~SkinHolder();
    void AddRef();
    void Release();

    void Initialize();
    void ExecuteSkinScript(std::string file);
    void LoadSkinImage(const std::string &key, const std::string &filename);
    void LoadSkinFont(const std::string &key, const std::string &filename);
    std::shared_ptr<Image> GetSkinImage(const std::string &key);
    std::shared_ptr<Font> GetSkinFont(const std::string &key);
};

class ExecutionManager final
{
private:
    std::shared_ptr<Setting> SharedSetting;
    std::shared_ptr<AngelScript> ScriptInterface;
    std::vector<std::shared_ptr<Scene>> Scenes;
    std::shared_ptr<KeyState> SharedKeyState;
    std::vector<std::string> SkinNames;
    std::unique_ptr<SkinHolder> Skin;

public:
    ExecutionManager(std::shared_ptr<Setting> setting);

    void EnumerateSkins();
    void Tick(double delta);
    void Draw();
    void AddScene(std::shared_ptr<Scene> scene);
    std::shared_ptr<ScriptScene> CreateSceneFromScriptType(asITypeInfo *type);
    std::shared_ptr<ScriptScene> CreateSceneFromScriptObject(asIScriptObject *obj);
    inline int GetSceneCount() { return Scenes.size(); }
    inline KeyState* GetKeyState() { return SharedKeyState.get(); }
    inline AngelScript* GetScriptInterface() { return ScriptInterface.get(); }

    void ExecuteSkin();
    void ExecuteSystemMenu();

private:
    bool CheckSkinStructure(boost::filesystem::path name);
    void UpdateKeyState();
};

SkinHolder* GetSkinObject();