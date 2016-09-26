#include "ScriptSprite.h"

SSprite::SSprite()
{
    ZIndex = 0;
    Color = Colors::White;
}

SSprite::~SSprite()
{
    if (Image) Image->Release();
}

void SSprite::AddRef()
{
    ++Reference;
}

void SSprite::Release()
{
    if (--Reference == 0) delete this;
}

void SSprite::Tick(double delta)
{
}

void SSprite::Draw()
{

}

SSprite * SSprite::Factory()
{
    auto result = new SSprite();
    result->AddRef();
    return result;
}

template<typename T>
void RegisterSpriteBasic(asIScriptEngine *engine, const char *name)
{
    engine->RegisterObjectType(SU_IF_SPRITE, 0, asOBJ_REF);
    engine->RegisterObjectBehaviour(SU_IF_SPRITE, asBEHAVE_FACTORY, SU_IF_SPRITE "@ f()", asFUNCTION(T::Factory), asCALL_CDECL);
    engine->RegisterObjectBehaviour(SU_IF_SPRITE, asBEHAVE_ADDREF, "void f()", asMETHOD(T, AddRef), asCALL_THISCALL);
    engine->RegisterObjectBehaviour(SU_IF_SPRITE, asBEHAVE_RELEASE, "void f()", asMETHOD(T, Release), asCALL_THISCALL);

    engine->RegisterObjectProperty(name, SU_IF_COLOR " Color", asOFFSET(T, Color));
    engine->RegisterObjectProperty(name, "int Z", asOFFSET(T, ZIndex));
    engine->RegisterObjectProperty(name, SU_IF_TF2D " Transform", asOFFSET(T, Transform));

    engine->RegisterObjectMethod(name, "void Draw()", asMETHOD(T, Draw), asCALL_CDECL_OBJFIRST);
}

//“¯Žž‚ÉColor‚ÆTransform2D‚à
void SSprite::RegisterType(asIScriptEngine * engine)
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

    //Color
    engine->RegisterObjectType(SU_IF_COLOR, sizeof(ColorTint), asOBJ_VALUE | asOBJ_APP_CLASS_CD);
    engine->RegisterObjectBehaviour(SU_IF_COLOR, asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(SpriteCtorColorTint), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectBehaviour(SU_IF_COLOR, asBEHAVE_DESTRUCT, "void f()", asFUNCTION(SpriteDtorColorTint), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectProperty(SU_IF_COLOR, "uint8 A", asOFFSET(ColorTint, A));
    engine->RegisterObjectProperty(SU_IF_COLOR, "uint8 R", asOFFSET(ColorTint, R));
    engine->RegisterObjectProperty(SU_IF_COLOR, "uint8 G", asOFFSET(ColorTint, G));
    engine->RegisterObjectProperty(SU_IF_COLOR, "uint8 B", asOFFSET(ColorTint, B));

    RegisterSpriteBasic<SSprite>(engine, SU_IF_SPRITE);
}

void RegisterScriptSprite(asIScriptEngine * engine)
{
    SSprite::RegisterType(engine);
}
