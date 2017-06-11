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
    for (auto &i : sprites) i->Release();
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

void ScriptScene::AddSprite(SSprite * sprite)
{
    //sprite->AddRef();
    spritesPending.push_back(sprite);
}

void ScriptScene::AddCoroutine(Coroutine * co)
{
    coroutinesPending.push_back(co);
}

void ScriptScene::Tick(double delta)
{
    TickSprite(delta);
    TickCoroutine(delta);
    auto func = sceneType->GetMethodByDecl("void Tick(double)");
    context->Prepare(func);
    context->SetObject(sceneObject);
    context->Execute();
}

void ScriptScene::OnEvent(const string &message)
{
    auto func = sceneType->GetMethodByDecl("void OnEvent(const string &in)");
    if (!func) return;
    auto evc = manager->GetScriptInterfaceUnsafe()->GetEngine()->CreateContext();
    evc->Prepare(func);
    evc->SetObject(sceneObject);
    evc->Execute();
    evc->Release();
}

void ScriptScene::Draw()
{
    DrawSprite();
    auto func = sceneType->GetMethodByDecl("void Draw()");
    context->Prepare(func);
    context->SetObject(sceneObject);
    context->Execute();
}

bool ScriptScene::IsDead()
{
    return false;
}

void ScriptScene::Disappear()
{}

void ScriptScene::TickCoroutine(double delta)
{
    for (auto& coroutine : coroutinesPending) {
        coroutine->context = context->GetEngine()->CreateContext();
        coroutine->context->SetUserData(this, SU_UDTYPE_SCENE);
        coroutine->context->SetUserData(&coroutine->wait, SU_UDTYPE_WAIT);
        coroutine->context->Prepare(coroutine->function);
        coroutine->context->SetObject(coroutine->object);
        coroutines.push_back(coroutine);
    }
    coroutinesPending.clear();

    auto i = coroutines.begin();
    while (i != coroutines.end()) {
        auto c = *i;
        switch (c->wait.type) {
            case WaitType::Frame:
                c->wait.frames -= 1;
                if (c->wait.frames > 0) {
                    i++;
                    continue;
                }
                break;
            case WaitType::Time:
                c->wait.time -= delta;
                if (c->wait.time > 0.0) {
                    i++;
                    continue;
                }
                break;
            default:
                WriteDebugConsole("Broken Wait Data!\n");
                abort();
                break;
        }
        auto result = c->context->Execute();
        if (result == asEXECUTION_FINISHED) {
            auto e = c->context->GetEngine();
            c->context->Release();
            c->function->Release();
            e->ReleaseScriptObject(c->object, c->type);
            delete c;
            i = coroutines.erase(i);
        } else if (result == asEXECUTION_EXCEPTION) {
            int col, row;
            const char *at;
            row = c->context->GetExceptionLineNumber(&col, &at);
            ostringstream str;
            str << col << ", " << row << ": " << at << endl;
            WriteDebugConsole(str.str().c_str());
            WriteDebugConsole(c->context->GetExceptionString());
            abort();
        } else {
            i++;
        }
    }
}

void ScriptScene::TickSprite(double delta)
{
    for (auto& sprite : spritesPending) sprites.emplace(sprite);
    spritesPending.clear();
    auto i = sprites.begin();
    while (i != sprites.end()) {
        (*i)->Tick(delta);
        if ((*i)->IsDead) {
            (*i)->Release();
            i = sprites.erase(i);
        } else {
            i++;
        }
    }
}

void ScriptScene::DrawSprite()
{
    for (auto& i : sprites) i->Draw();
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

ScriptCoroutineScene::~ScriptCoroutineScene()
{
    runningContext->Release();
    for (auto& i : coroutines) {
        auto e = i->context->GetEngine();
        i->context->Release();
        i->function->Release();
        e->ReleaseScriptObject(i->object, i->type);
        delete i;
    }
    coroutines.clear();
}

void ScriptCoroutineScene::Tick(double delta)
{
    TickSprite(delta);
    TickCoroutine(delta);
    //Run()
    switch (wait.type) {
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
    if (result != asEXECUTION_SUSPENDED)
        finished = true;
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

void ScriptCoroutineScene::Disappear()
{
    finished = true;
}

void RegisterScriptScene(ExecutionManager *exm)
{
    auto engine = exm->GetScriptInterfaceUnsafe()->GetEngine();

    engine->RegisterFuncdef("void " SU_IF_COROUTINE "()");

    engine->RegisterInterface(SU_IF_SCENE);
    engine->RegisterInterfaceMethod(SU_IF_SCENE, "void Initialize()");
    engine->RegisterInterfaceMethod(SU_IF_SCENE, "void Tick(double)");
    engine->RegisterInterfaceMethod(SU_IF_SCENE, "void Draw()");

    engine->RegisterInterface(SU_IF_COSCENE);
    engine->RegisterInterfaceMethod(SU_IF_COSCENE, "void Initialize()");
    engine->RegisterInterfaceMethod(SU_IF_COSCENE, "void Run()");
    engine->RegisterInterfaceMethod(SU_IF_COSCENE, "void Draw()");

    engine->RegisterGlobalFunction("bool IsKeyHeld(int)", asFUNCTION(ScriptSceneIsKeyHeld), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool IsKeyTriggered(int)", asFUNCTION(ScriptSceneIsKeyTriggered), asCALL_CDECL);
    engine->RegisterGlobalFunction("void RunCoroutine(" SU_IF_COROUTINE "@, const string &in)", asFUNCTION(ScriptSceneRunCoroutine), asCALL_CDECL);
    engine->RegisterGlobalFunction("void KillCoroutine(const string &in)", asFUNCTION(ScriptSceneKillCoroutine), asCALL_CDECL);
    engine->RegisterGlobalFunction("void Disappear()", asFUNCTION(ScriptSceneDisappear), asCALL_CDECL);
    engine->RegisterGlobalFunction("void AddSprite(" SU_IF_SPRITE "@)", asFUNCTION(ScriptSceneAddSprite), asCALL_CDECL);
    engine->RegisterGlobalFunction("void AddScene(" SU_IF_SCENE "@)", asFUNCTION(ScriptSceneAddScene), asCALL_CDECL);
    engine->RegisterGlobalFunction("void AddScene(" SU_IF_COSCENE "@)", asFUNCTION(ScriptSceneAddScene), asCALL_CDECL);
}

int ScriptSceneGetIndex()
{
    return 0;
}

void ScriptSceneSetIndex(int index)
{
    auto ctx = asGetActiveContext();
    auto psc = static_cast<ScriptScene*>(ctx->GetUserData(SU_UDTYPE_SCENE));
    if (!psc) {
        ScriptSceneWarnOutOf("Scene Class", ctx);
        return;
    }
    psc->SetIndex(index);
}

int ScriptSceneGetIndex()
{
    auto ctx = asGetActiveContext();
    auto psc = static_cast<ScriptScene*>(ctx->GetUserData(SU_UDTYPE_SCENE));
    if (!psc) {
        ScriptSceneWarnOutOf("Scene Class", ctx);
        return 0;
    }
    return psc->GetIndex();
}

// Scene用メソッド

bool ScriptSceneIsKeyHeld(int keynum)
{
    auto ctx = asGetActiveContext();
    auto psc = static_cast<ScriptScene*>(ctx->GetUserData(SU_UDTYPE_SCENE));
    if (!psc) {
        ScriptSceneWarnOutOf("Scene Class", ctx);
        return false;
    }
    return psc->GetManager()->GetKeyStateUnsafe()->Current[keynum];
}

bool ScriptSceneIsKeyTriggered(int keynum)
{
    auto ctx = asGetActiveContext();
    auto psc = static_cast<ScriptScene*>(ctx->GetUserData(SU_UDTYPE_SCENE));
    if (!psc) {
        ScriptSceneWarnOutOf("Scene Class", ctx);
        return false;
    }
    return  psc->GetManager()->GetKeyStateUnsafe()->Trigger[keynum];
}

void ScriptSceneAddScene(asIScriptObject *sceneObject)
{
    auto ctx = asGetActiveContext();
    auto psc = static_cast<ScriptScene*>(ctx->GetUserData(SU_UDTYPE_SCENE));
    if (!psc) {
        ScriptSceneWarnOutOf("Scene Class", ctx);
        return;
    }
    psc->GetManager()->CreateSceneFromScriptObject(sceneObject);
}

void ScriptSceneAddSprite(SSprite * sprite)
{
    auto ctx = asGetActiveContext();
    auto psc = static_cast<ScriptCoroutineScene*>(ctx->GetUserData(SU_UDTYPE_SCENE));
    if (!psc) {
        ScriptSceneWarnOutOf("Scene Class", ctx);
        return;
    }
    psc->AddSprite(sprite);
}

void ScriptSceneRunCoroutine(asIScriptFunction *cofunc, const string &name)
{
    auto ctx = asGetActiveContext();
    auto psc = static_cast<ScriptCoroutineScene*>(ctx->GetUserData(SU_UDTYPE_SCENE));
    if (!psc) {
        ScriptSceneWarnOutOf("Scene Class", ctx);
        return;
    }
    if (!cofunc || cofunc->GetFuncType() != asFUNC_DELEGATE) return;
    auto *c = new Coroutine;
    c->name = name;
    c->function = cofunc->GetDelegateFunction();
    c->function->AddRef();
    c->object = cofunc->GetDelegateObject();
    c->type = cofunc->GetDelegateObjectType();
    c->wait.type = WaitType::Time;
    c->wait.time = 0;
    psc->AddCoroutine(c);
}

void ScriptSceneKillCoroutine(const std::string &name)
{
    auto ctx = asGetActiveContext();
    auto psc = static_cast<ScriptCoroutineScene*>(ctx->GetUserData(SU_UDTYPE_SCENE));
    if (!psc) {
        ScriptSceneWarnOutOf("Scene Class", ctx);
        return;
    }
    if (name == "") {
        for (auto& i : psc->coroutines) {
            auto e = i->context->GetEngine();
            i->context->Release();
            i->function->Release();
            e->ReleaseScriptObject(i->object, i->type);
            delete i;
        }
        psc->coroutines.clear();
    } else {
        auto i = psc->coroutines.begin();
        while (i != psc->coroutines.end()) {
            auto c = *i;
            if (c->name == name) {
                auto e = c->context->GetEngine();
                c->context->Release();
                c->function->Release();
                e->ReleaseScriptObject(c->object, c->type);
                delete c;
                i = psc->coroutines.erase(i);
            } else {
                ++i;
            }
        }
    }
}

void ScriptSceneDisappear()
{
    auto ctx = asGetActiveContext();
    auto psc = static_cast<ScriptCoroutineScene*>(ctx->GetUserData(SU_UDTYPE_SCENE));
    if (!psc) {
        ScriptSceneWarnOutOf("Scene Class", ctx);
        return;
    }
    psc->Disappear();
}
