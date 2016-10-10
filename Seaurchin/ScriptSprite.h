#pragma once

#include "Sprite.h"
#include "ScriptResource.h"

#define SU_IF_COLOR "Color"
#define SU_IF_TF2D "Transform2D"
#define SU_IF_SHAPETYPE "ShapeType"

#define SU_IF_SPRITE "Sprite"
#define SU_IF_SHAPE "Shape"
#define SU_IF_TXTSPRITE "TextSprite"
#define SU_IF_SYHSPRITE "SynthSprite"
#define SU_IF_CLPSPRITE "ClipSprite"

class ScriptSpriteMover;
//基底がImageSpriteでもいい気がしてるんだよね正直
class SSprite
{
protected:
    int Reference;
    ScriptSpriteMover *mover;

    void CopyParameterFrom(SSprite *original);

public:
    //値(CopyParameterFromで一括)
    Transform2D Transform;
    int32_t ZIndex;
    ColorTint Color;
    bool IsDead = false;
    bool HasAlpha = true;
    //参照(手動コピー)
    SImage *Image = nullptr;
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

//任意の多角形などを表示できる
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

//文字列をスプライトとして扱います
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
    STextSprite *Clone();

    static STextSprite* Factory();
    static STextSprite* Factory(SFont *img, const std::string &str);
    static void RegisterType(asIScriptEngine *engine);
};

//画像を任意のスプライトから合成してウェイできます
class SSynthSprite : public SSprite
{
protected:
    SRenderTarget *Target = nullptr;
    int Width = 0;
    int Height = 0;

public:
    SSynthSprite(int w, int h);
    ~SSynthSprite();
    inline int get_Width() { return Width; }
    inline int get_Height() { return Height; }

    void Clear();
    void Transfer(SSprite *sprite);
    void Draw() override;
    SSynthSprite *Clone();

    static SSynthSprite *Factory(int w, int h);
    static void RegisterType(asIScriptEngine *engine);
};

//画像を任意のスプライトから合成してウェイできます
class SClippingSprite : public SSynthSprite
{
protected:
    double U1;
    double V1;
    double U2;
    double V2;
    SRenderTarget *ActualTarget = nullptr;

public:
    SClippingSprite(int w, int h);

    void SetRange(double tx, double ty, double w, double h);
    void Draw() override;
    SClippingSprite *Clone();

    static SSynthSprite *Factory(int w, int h);
    static void RegisterType(asIScriptEngine *engine);
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

//Sprite系全部
//ColorTint GetColorTint(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
void RegisterScriptSprite(asIScriptEngine *engine);