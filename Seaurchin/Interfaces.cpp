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
    engine->RegisterObjectType(SU_IF_TF2D, sizeof(Transform2D), asOBJ_VALUE | asOBJ_APP_CLASS_CD);
    engine->RegisterObjectBehaviour(SU_IF_TF2D, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(SpriteCtorTransform2D), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectBehaviour(SU_IF_TF2D, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(SpriteDtorTransform2D), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectProperty(SU_IF_TF2D, "double X", asOFFSET(Transform2D, X));
    engine->RegisterObjectProperty(SU_IF_TF2D, "double Y", asOFFSET(Transform2D, Y));
    engine->RegisterObjectProperty(SU_IF_TF2D, "double Angle", asOFFSET(Transform2D, Angle));
    engine->RegisterObjectProperty(SU_IF_TF2D, "double OriginX", asOFFSET(Transform2D, OriginX));
    engine->RegisterObjectProperty(SU_IF_TF2D, "double OriginY", asOFFSET(Transform2D, OriginY));
    engine->RegisterObjectProperty(SU_IF_TF2D, "double ScaleX", asOFFSET(Transform2D, ScaleX));
    engine->RegisterObjectProperty(SU_IF_TF2D, "double ScaleY", asOFFSET(Transform2D, ScaleY));

    //Image
    InterfacesRegisterSharedClass<Image>(engine, SU_IF_IMAGE);

    //VirtualFont
    InterfacesRegisterSharedClass<VirtualFont>(engine, SU_IF_VFONT);
    //engine->RegisterObjectMethod(SU_IF_VFONT, "void DrawRaw(const string & in, double, double)", CALLER(VirtualFont, DrawRaw), asCALL_CDECL_OBJFIRST);

    //Sprite
    InterfacesRegisterSharedClass<Sprite>(engine, SU_IF_SPRITE);
    engine->RegisterObjectMethod(SU_IF_SPRITE, SU_IF_TF2D "& get_Transform() const", REF_GETTER(Sprite, Transform), asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod(SU_IF_SPRITE, "void set_Transform(const " SU_IF_TF2D "& in)", REF_SETTER(Sprite, Transform), asCALL_CDECL_OBJFIRST);


    //ImageSprite
    InterfacesRegisterSharedClass<ImageSprite>(engine, SU_IF_IMGSPRITE);
    engine->RegisterObjectMethod(SU_IF_IMGSPRITE, SU_IF_TF2D "& get_Transform() const", REF_GETTER(ImageSprite, Transform), asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod(SU_IF_IMGSPRITE, "void set_Transform(const " SU_IF_TF2D "& in)", REF_SETTER(ImageSprite, Transform), asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod(SU_IF_IMGSPRITE, SU_IF_IMAGE " get_Source() const", GETTER(ImageSprite, Source), asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod(SU_IF_IMGSPRITE, "void set_Source(const " SU_IF_IMAGE ")", SETTER(ImageSprite, Source), asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod(SU_IF_IMGSPRITE, "void Draw()", CALLER(ImageSprite, Draw), asCALL_CDECL_OBJFIRST);
}

void InterfacesRegisterGlobalFunction(asIScriptEngine *engine)
{
    engine->RegisterGlobalFunction("void WriteDebugConsole(const string &in)", asFUNCTION(WriteDebugConsoleU), asCALL_CDECL);
    engine->RegisterGlobalFunction(SU_IF_VFONT " CreateVirtualFont(const string & in, int)", asFUNCTION(CreateVirtualFont), asCALL_CDECL);
    engine->RegisterGlobalFunction(SU_IF_IMAGE " LoadSystemImage(const string &in)", asFUNCTION(LoadSystemImage), asCALL_CDECL);
    engine->RegisterGlobalFunction(SU_IF_SPRITE " CreateSprite(int)", asFUNCTION(SpriteFactorySprite), asCALL_CDECL);
    engine->RegisterGlobalFunction(SU_IF_IMGSPRITE " CreateImageSprite()", asFUNCTION(SpriteFactoryImageSprite), asCALL_CDECL);
    engine->RegisterGlobalFunction("void DrawRawString(" SU_IF_VFONT ", const string & in, double, double)", asFUNCTION(DrawRawString), asCALL_CDECL);
}

void InterfaceRegisterSceneFunction(asIScriptEngine *engine)
{
    engine->RegisterGlobalFunction("void YieldTime(double)", asFUNCTION(ScriptSceneYieldTime), asCALL_CDECL);
    engine->RegisterGlobalFunction("void YieldFrame(int64)", asFUNCTION(ScriptSceneYieldFrames), asCALL_CDECL);
}

void InterfacesRegisterDrawFunction(asIScriptEngine *engine)
{

}