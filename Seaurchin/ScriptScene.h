#pragma once

#include "Scene.h"
#include "ScriptSprite.h"
#include "ScriptSpriteManager.h"
#include "ScriptFunction.h"

#define SU_IF_SCENE "Scene"
#define SU_IF_COSCENE "CoroutineScene"
#define SU_IF_COROUTINE "Coroutine"

typedef struct
{
    std::string name;
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
    std::multiset<SSprite*, SSprite::Comparator> sprites;
    std::vector<SSprite*> spritesPending;
    std::list<Coroutine*> coroutines;
    std::list<Coroutine*> coroutinesPending;

    void TickCoroutine(double delta);
    void TickSprite(double delta);
    void DrawSprite();

public:
    ScriptScene(asIScriptObject *scene);
    ~ScriptScene();

    void Initialize() override;

    void AddSprite(SSprite *sprite);
    void AddCoroutine(Coroutine *co);
    void Tick(double delta) override;
    void Draw() override;
    bool IsDead() override;
    virtual void Disappear();

    friend void ScriptSceneKillCoroutine(const std::string &name);
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
    
    
    void Tick(double delta) override;
    void Initialize() override;
    bool IsDead() override;
    void Disappear() override;

};

class ExecutionManager;
void RegisterScriptScene(ExecutionManager *exm);

bool ScriptSceneIsKeyHeld(int keynum);
bool ScriptSceneIsKeyTriggered(int keynum);
void ScriptSceneAddScene(asIScriptObject *sceneObject);
void ScriptSceneAddSprite(SSprite *sprite);
void ScriptSceneRunCoroutine(asIScriptFunction *cofunc, const std::string &name);
void ScriptSceneKillCoroutine(const std::string &name);
void ScriptSceneDisappear();