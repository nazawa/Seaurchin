#pragma once

#include "Scene.h"

enum WaitType
{
    Frame,
    Time,
};

typedef struct
{
    WaitType type;
    union
    {
        double time;
        int64_t frames;
    };
} CoroutineWait;

typedef struct
{
    void *object;
    asIScriptContext *context;
    asITypeInfo *type;
    asIScriptFunction *function;
    CoroutineWait wait;
} Coroutine;

class ScriptScene : public Scene
{
    typedef Scene base;
protected:
    asIScriptContext *context;
    asIScriptObject *sceneObject;
    asITypeInfo *sceneType;

public:
    ScriptScene(asIScriptObject *scene);
    ~ScriptScene();

    void Initialize() override;

    void Tick(double delta) override;
    void Draw() override;
    bool IsDead() override;

};

class ScriptCoroutineScene : public ScriptScene
{
    typedef ScriptScene base;
protected:
    asIScriptContext *runningContext;
    CoroutineWait wait;
    bool finished = false;

public:
    ScriptCoroutineScene(asIScriptObject *scene);
    ~ScriptCoroutineScene();
    std::list<Coroutine*> coroutines;
    
    void Tick(double delta) override;
    void Initialize() override;
    bool IsDead() override;

};

void ScriptSceneWarnOutOf(std::string type, asIScriptContext *ctx);
void ScriptSceneYieldTime(double time);
void ScriptSceneYieldFrames(int64_t frames);
bool ScriptSceneIsKeyHeld(int keynum);
bool ScriptSceneIsKeyTriggered(int keynum);
void ScriptSceneAddMove(std::shared_ptr<Sprite> sprite, const std::string &move);
void ScriptSceneAddScene(asIScriptObject *sceneObject);
void ScriptSceneRunCoroutine(asIScriptFunction *cofunc);