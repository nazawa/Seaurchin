#pragma once

#include "Sprite.h"
#include "ScriptResource.h"

#define SU_IF_COLOR "Color"
#define SU_IF_TF2D "Transform2D"

#define SU_IF_SPRITE "Sprite"
#define SU_IF_TXTSPRITE "TextSprite"

//äÓíÍÇ™ImageSpriteÇ≈Ç‡Ç¢Ç¢ãCÇ™ÇµÇƒÇÈÇÒÇæÇÊÇÀê≥íº
class SSprite
{
protected:
    int Reference;
    
public:
    Transform2D Transform;
    int32_t ZIndex;
    ColorTint Color;
    SImage *Image = nullptr;
    bool HasAlpha = true;
    void set_Image(SImage *img);
    const SImage* get_Image();

    SSprite();
    ~SSprite();
    void AddRef();
    void Release();

    void Tick(double delta);
    void Draw();

    static SSprite* Factory();
    static void RegisterType(asIScriptEngine *engine);
};

enum SShapeType
{
    Pixel,
    Box,
    BoxFill,
    Oval,
    OvalFill,
};

class SShape : public SSprite
{
public:
    SShapeType Type;
    double Width;
    double Height;

    void Draw(); //override;
};


template<typename T>
void RegisterSpriteBasic(asIScriptEngine *engine, const char *name)
{
    using namespace std;
    engine->RegisterObjectType(name, 0, asOBJ_REF);
    engine->RegisterObjectBehaviour(name, asBEHAVE_FACTORY, (string(name) + "@ f()").c_str(), asFUNCTION(T::Factory), asCALL_CDECL);
    engine->RegisterObjectBehaviour(name, asBEHAVE_ADDREF, "void f()", asMETHOD(T, AddRef), asCALL_THISCALL);
    engine->RegisterObjectBehaviour(name, asBEHAVE_RELEASE, "void f()", asMETHOD(T, Release), asCALL_THISCALL);
    
    engine->RegisterObjectProperty(name, SU_IF_COLOR " Color", asOFFSET(T, Color));
    engine->RegisterObjectProperty(name, "int Z", asOFFSET(T, ZIndex));
    engine->RegisterObjectProperty(name, SU_IF_TF2D " Transform", asOFFSET(T, Transform));
    engine->RegisterObjectMethod(name, "void SetImage(" SU_IF_IMAGE "@)", asMETHOD(T, set_Image), asCALL_THISCALL);
    //engine->RegisterObjectMethod(name, SU_IF_IMAGE "@ get_Image()", asMETHOD(T, get_Image), asCALL_THISCALL);

    //engine->RegisterObjectMethod(name, "void Tick(double)", asMETHOD(T, Tick), asCALL_THISCALL);
    engine->RegisterObjectMethod(name, "void Draw()", asMETHOD(T, Draw), asCALL_THISCALL);
}

//SpriteånëSïî
void RegisterScriptSprite(asIScriptEngine *engine);