#pragma once

#include <Windows.h>
#include <DxLib.h>
#include <angelscript.h>
#include <vector>
#include <memory>
#include <algorithm>

#include "Scene.h"

class ScriptScene : public Scene
{
private:
    asIScriptObject *sceneObject;
    asITypeInfo *sceneType;
    bool finished;

public:
    ScriptScene(asIScriptObject *scene);
    ~ScriptScene();

    virtual void Initialize();

    void Tick(double delta) override;
    void Draw() override;
    bool IsDead() override;

};