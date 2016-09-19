#pragma once

#include <Windows.h>
#include <DxLib.h>
#include <angelscript.h>
#include <scriptbuilder\scriptbuilder.h>
#include <scriptarray\scriptarray.h>
#include <scriptmath\scriptmath.h>
#include <scriptmath\scriptmathcomplex.h>
#include <scriptstdstring\scriptstdstring.h>
#include <vector>
#include <memory>
#include <algorithm>

#include "Scene.h"
#include "AngelScriptManager.h"

class SceneManager final
{
private:
    std::shared_ptr<AngelScript> ScriptInterface;
    std::vector<std::shared_ptr<Scene>> Scenes;

public:
    SceneManager(std::shared_ptr<AngelScript> script);
    ~SceneManager();

    void Tick(double delta);
    void Draw();

    void AddScene(std::shared_ptr<Scene> scene);
    inline int GetSceneCount() { return Scenes.size(); }
    
};