#include "ScriptScene.h"

#include "Config.h"
#include "ExecutionManager.h"
#include "Misc.h"

using namespace std;
using namespace boost::filesystem;

ScriptScene::ScriptScene(asIScriptObject *scene)
{
    sceneObject = scene;
    sceneObject->AddRef();

    sceneType = sceneObject->GetObjectType();
    sceneType->AddRef();

    auto eng = sceneObject->GetEngine();
    context = eng->CreateContext();
    context->SetUserData(this, SU_UDTYPE_SCENE);
}

ScriptScene::~ScriptScene()
{
    context->Release();
    sceneType->Release();
    sceneObject->Release();
}

void ScriptScene::Initialize()
{
    auto func = sceneType->GetMethodByDecl("void Initialize()");
    context->Prepare(func);
    context->SetObject(sceneObject);
    context->Execute();
}

void ScriptScene::Tick(double delta)
{
    auto func = sceneType->GetMethodByDecl("void Tick(double)");
    spmanager.Tick(delta);
    context->Prepare(func);
    context->SetObject(sceneObject);
    context->Execute();
}

void ScriptScene::Draw()
{
    auto func = sceneType->GetMethodByDecl("void Draw()");
    context->Prepare(func);
    context->SetObject(sceneObject);
    context->Execute();
}

bool ScriptScene::IsDead()
{
    return false;
}

ScriptCoroutineScene::ScriptCoroutineScene(asIScriptObject *scene) : base(scene)
{
    auto eng = sceneObject->GetEngine();
    runningContext = eng->CreateContext();
    runningContext->SetUserData(this, SU_UDTYPE_SCENE);
    runningContext->SetUserData(&wait, SU_UDTYPE_WAIT);
    wait.type = WaitType::Time;
    wait.time = 0.0;
}

void ScriptCoroutineScene::Tick(double delta)
{
    spmanager.Tick(delta);

    //Run()
    switch (wait.type)
    {
    case WaitType::Frame:
        wait.frames -= 1;
        if (wait.frames > 0) return;
        break;
    case WaitType::Time:
        wait.time -= delta;
        if (wait.time > 0.0) return;
        break;
    }
    auto result = runningContext->Execute();
    if (result != asEXECUTION_SUSPENDED) finished = true;
}

void ScriptCoroutineScene::Initialize()
{
    base::Initialize();
    auto func = sceneType->GetMethodByDecl("void Run()");
    runningContext->Prepare(func);
    runningContext->SetObject(sceneObject);
}

bool ScriptCoroutineScene::IsDead()
{
    return finished;
}

// Scene用メソッド

void ScriptSceneYieldTime(double time)
{
    auto ctx = asGetActiveContext();
    auto pcw = (CoroutineWait*)ctx->GetUserData(SU_UDTYPE_WAIT);
    if (!pcw)
    {
        ScriptSceneWarnOutOf("Coroutine Function", ctx);
        return;
    }
    pcw->type = WaitType::Time;
    pcw->time = time;
    ctx->Suspend();
}

void ScriptSceneYieldFrames(int64_t frames)
{
    auto ctx = asGetActiveContext();
    auto pcw = (CoroutineWait*)ctx->GetUserData(SU_UDTYPE_WAIT);
    if (!pcw)
    {
        ScriptSceneWarnOutOf("Coroutine Function", ctx);
        return;
    }
    pcw->type = WaitType::Frame;
    pcw->frames = frames;
    ctx->Suspend();
}

bool ScriptSceneIsKeyHeld(int keynum)
{
    auto ctx = asGetActiveContext();
    auto psc = (Scene*)ctx->GetUserData(SU_UDTYPE_SCENE);
    if (!psc)
    {
        ScriptSceneWarnOutOf("Scene Class", ctx);
        return false;
    }
    return psc->GetManager()->GetKeyState()->Current[keynum];
}

bool ScriptSceneIsKeyTriggered(int keynum)
{
    auto ctx = asGetActiveContext();
    auto psc = (Scene*)ctx->GetUserData(SU_UDTYPE_SCENE);
    if (!psc)
    {
        ScriptSceneWarnOutOf("Scene Class", ctx);
        return false;
    }
    return  psc->GetManager()->GetKeyState()->Trigger[keynum];
}

void ScriptSceneAddMove(shared_ptr<Sprite> sprite, const string &move)
{
    auto ctx = asGetActiveContext();
    auto psc = static_cast<Scene*>(ctx->GetUserData(SU_UDTYPE_SCENE));
    if (!psc)
    {
        ScriptSceneWarnOutOf("Scene Class", ctx);
        return;
    }
    psc->GetSpriteManager()->AddMove(sprite, move);
}

void ScriptSceneAddScene(asIScriptObject *sceneObject)
{
    auto ctx = asGetActiveContext();
    auto psc = static_cast<Scene*>(ctx->GetUserData(SU_UDTYPE_SCENE));
    if (!psc)
    {
        ScriptSceneWarnOutOf("Scene Class", ctx);
        return;
    }
    psc->GetManager()->CreateSceneFromScriptObject(sceneObject);
}
