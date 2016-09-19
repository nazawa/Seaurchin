#include "ScriptScene.h"

ScriptScene::ScriptScene(asIScriptObject *scene)
{
    sceneObject = scene;
    sceneObject->AddRef();

    sceneType = sceneObject->GetObjectType();
    sceneType->AddRef();
}

ScriptScene::~ScriptScene()
{
    sceneType->Release();
    sceneObject->Release();
}

void ScriptScene::Initialize()
{
}

void ScriptScene::Tick(double delta)
{
    auto func = sceneType->GetMethodByDecl("void Tick(double)");
    if (!func) return;

}

void ScriptScene::Draw()
{
}

bool ScriptScene::IsDead()
{
    return false;
}
