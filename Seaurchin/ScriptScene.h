#pragma once


#include "Config.h"
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

class ScriptScene : public Scene
{
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