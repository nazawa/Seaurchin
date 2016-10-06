#pragma once

#include "Sprite.h"
#include "ScriptResource.h"

#define SU_IF_COLOR "Color"
#define SU_IF_TF2D "Transform2D"
#define SU_IF_SHAPETYPE "ShapeType"

#define SU_IF_SPRITE "Sprite"
#define SU_IF_SHAPE "Shape"
#define SU_IF_TXTSPRITE "TextSprite"

class ScriptSpriteMover;
//Šî’ê‚ªImageSprite‚Å‚à‚¢‚¢‹C‚ª‚µ‚Ä‚é‚ñ‚¾‚æ‚Ë³’¼
class SSprite
{
protected:
    int Reference;
    ScriptSpriteMover *mover;


public:
    Transform2D Transform;
    int32_t ZIndex;
    ColorTint Color;
    bool IsDead = false;
    SImage *Image = nullptr;
    bool HasAlpha = true;
    void set_Image(SImage *img);
    const SImage* get_Image();

    SSprite();
    virtual ~SSprite();
    void AddRef();
    void Release();

    inline void Dismiss() { IsDead = true; }
    inline void Revive() { IsDead = false; }
    void AddMove(const std::string &move);
    void Apply(const std::string &dict);
    void Apply(const CScriptDictionary &dict);
    virtual void Tick(double delta);
    virtual void Draw();
    SSprite* Clone();

    static SSprite* Factory();
    static SSprite* Factory(SImage *img);
    static void RegisterType(asIScriptEngine *engine);
    struct Comparator
    {
        inline bool operator()(const SSprite* lhs, const SSprite* rhs) const
        {
            return lhs->ZIndex < rhs->ZIndex;
        }
    };
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
    SShapeType Type = SShapeType::BoxFill;
    double Width = 32;
    double Height = 32;

    void Draw() override;

    static SShape* Factory();
    static void RegisterType(asIScriptEngine *engine);
};

class STextSprite : public SSprite
{
protected:
    SRenderTarget *Target = nullptr;

    void Refresh();
public:
    SFont *Font = nullptr;
    std::string Text = "";
    void set_Font(SFont* font);
    void set_Text(const std::string &txt);

    ~STextSprite() override;
    void Draw() override;

    static STextSprite* Factory();
    static STextSprite* Factory(SFont *img, const std::string &str);
    static void RegisterType(asIScriptEngine *engine);
};

enum SScrollRepetition
{
    Horizontal = 1,
    Vertical = 2
};

class SScrollSprite : public SSprite
{
protected:

public:
    double Width = 64;
    double Height = 64;
    SScrollRepetition type;
};

class SEffectSprite : public SSprite
{
protected:
    
public:
    SEffectSprite();
    ~SEffectSprite();

    void Draw() override;
    void Tick(double delta) override;
    void Play();
    void Reset();
    void Stop();

    static SEffectSprite* Factory();
    static void RegisterType(asIScriptEngine *engine);
};

template<typename T>
void RegisterSpriteBasic(asIScriptEngine *engine, const char *name)
{
    using namespace std;
    engine->RegisterObjectType(name, 0, asOBJ_REF);
    engine->RegisterObjectBehaviour(name, asBEHAVE_ADDREF, "void f()", asMETHOD(T, AddRef), asCALL_THISCALL);
    engine->RegisterObjectBehaviour(name, asBEHAVE_RELEASE, "void f()", asMETHOD(T, Release), asCALL_THISCALL);

    engine->RegisterObjectProperty(name, SU_IF_COLOR " Color", asOFFSET(T, Color));
    engine->RegisterObjectProperty(name, "bool HasAlpha", asOFFSET(T, HasAlpha));
    engine->RegisterObjectProperty(name, "int Z", asOFFSET(T, ZIndex));
    engine->RegisterObjectProperty(name, SU_IF_TF2D " Transform", asOFFSET(T, Transform));
    engine->RegisterObjectMethod(name, "void SetImage(" SU_IF_IMAGE "@)", asMETHOD(T, set_Image), asCALL_THISCALL);
    //engine->RegisterObjectMethod(name, SU_IF_IMAGE "@ get_Image()", asMETHOD(T, get_Image), asCALL_THISCALL);
    engine->RegisterObjectMethod(name, "void Dismiss()", asMETHOD(T, Dismiss), asCALL_THISCALL);
    engine->RegisterObjectMethod(name, "void Apply(const string &in)", asMETHODPR(T, Apply, (const std::string&), void), asCALL_THISCALL);
    engine->RegisterObjectMethod(name, "void Apply(const dictionary@)", asMETHODPR(T, Apply, (const CScriptDictionary&), void), asCALL_THISCALL);
    engine->RegisterObjectMethod(name, "void AddMove(const string &in)", asMETHOD(T, AddMove), asCALL_THISCALL);
    engine->RegisterObjectMethod(name, "void Tick(double)", asMETHOD(T, Tick), asCALL_THISCALL);
    engine->RegisterObjectMethod(name, "void Draw()", asMETHOD(T, Draw), asCALL_THISCALL);
}

template<typename From, typename To>
To* CastReferenceType(From *from)
{
    if (!from) return nullptr;
    To* result = dynamic_cast<To*>(from);
    if (result) result->AddRef();
    return result;
}

//SpriteŒn‘S•”
//ColorTint GetColorTint(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
void RegisterScriptSprite(asIScriptEngine *engine);