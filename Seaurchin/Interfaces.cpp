#include "Interfaces.h"

void InterfacesRegisterScene(asIScriptEngine *engine)
{
    engine->RegisterInterface(SU_IF_SCENE);
    engine->RegisterInterfaceMethod(SU_IF_SCENE, "void Initialize()");
    engine->RegisterInterfaceMethod(SU_IF_SCENE, "void Tick(double)");
    engine->RegisterInterfaceMethod(SU_IF_SCENE, "void Draw()");

    engine->RegisterInterface(SU_IF_COSCENE);
    engine->RegisterInterfaceMethod(SU_IF_COSCENE, "void Initialize()");
    engine->RegisterInterfaceMethod(SU_IF_COSCENE, "void Run()");
    engine->RegisterInterfaceMethod(SU_IF_COSCENE, "void Draw()");
}

void InterfacesRegisterSprite(asIScriptEngine *engine)
{
    //Transform2D
    engine->RegisterObjectType(SU_IF_TF2D, sizeof(Transform2D), asOBJ_VALUE);
    engine->RegisterObjectBehaviour(SU_IF_TF2D, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(SpriteCtorTransform2D), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectBehaviour(SU_IF_TF2D, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(SpriteDtorTransform2D), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectProperty(SU_IF_TF2D, "double X", asOFFSET(Transform2D, X));
    engine->RegisterObjectProperty(SU_IF_TF2D, "double Y", asOFFSET(Transform2D, Y));
    engine->RegisterObjectProperty(SU_IF_TF2D, "double Angle", asOFFSET(Transform2D, Angle));
    engine->RegisterObjectProperty(SU_IF_TF2D, "double OriginX", asOFFSET(Transform2D, OriginX));
    engine->RegisterObjectProperty(SU_IF_TF2D, "double OriginY", asOFFSET(Transform2D, OriginY));
    engine->RegisterObjectProperty(SU_IF_TF2D, "double ScaleX", asOFFSET(Transform2D, ScaleX));
    engine->RegisterObjectProperty(SU_IF_TF2D, "double ScaleY", asOFFSET(Transform2D, ScaleY));

    InterfacesRegisterSharedClass<Sprite>(engine, SU_IF_SPRITE);
    engine->RegisterObjectMethod(SU_IF_SPRITE, SU_IF_TF2D " get_Transform() const", GETTER(Sprite, Transform), asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod(SU_IF_SPRITE, "void set_Transform(const " SU_IF_TF2D ")", SETTER(Sprite, Transform), asCALL_CDECL_OBJFIRST);

}

void InterfacesRegisterGlobalFunction(asIScriptEngine *engine)
{
    engine->RegisterGlobalFunction("void WriteDebugConsole(const string &in)", asFUNCTION(WriteDebugConsoleU), asCALL_CDECL);
}

void InterfaceRegisterSceneFunction(asIScriptEngine *engine)
{
    engine->RegisterGlobalFunction("void YieldTime(double)", asFUNCTION(ScriptSceneYieldTime), asCALL_CDECL);
    engine->RegisterGlobalFunction("void YieldFrame(int64)", asFUNCTION(ScriptSceneYieldFrames), asCALL_CDECL);
}

void InterfacesRegisterDrawFunction(asIScriptEngine *engine)
{
    
}