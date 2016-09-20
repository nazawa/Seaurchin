#pragma once

#include "AngelScriptManager.h"
#include "Scene.h"
#include "ScriptScene.h"
#include "Interfaces.h"

typedef struct
{
    char Current[256];
    char Last[256];
    char Trigger[256];
} KeyState;

class ExecutionManager final
{
private:
    std::shared_ptr<AngelScript> ScriptInterface;
    std::vector<std::shared_ptr<Scene>> Scenes;
    KeyState SharedKeyState;

public:
    ExecutionManager();

    void EnumerateSkins();
    void InitializeExecution();
    void Tick(double delta);
    void Draw();
    void AddScene(std::shared_ptr<Scene> scene);
    std::shared_ptr<ScriptScene> CreateSceneFromScriptType(asITypeInfo *type);
    inline int GetSceneCount() { return Scenes.size(); }

    void StartSystemMenu();

private:
    bool CheckSkinStructure(boost::filesystem::path name);
};
