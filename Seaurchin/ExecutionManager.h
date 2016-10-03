#pragma once

#include "Setting.h"
#include "AngelScriptManager.h"
#include "DxLibResource.h"
#include "Scene.h"
#include "ScriptScene.h"
#include "SkinHolder.h"

class ExecutionManager final
{
private:
    std::shared_ptr<Setting> SharedSetting;
    std::shared_ptr<AngelScript> ScriptInterface;
    std::vector<std::shared_ptr<Scene>> Scenes;
    std::shared_ptr<KeyState> SharedKeyState;
    std::vector<std::string> SkinNames;
    std::unique_ptr<SkinHolder> Skin;
    Effekseer::Manager *Effect2D;
    Effekseer::Manager *Effect3D;

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

