#pragma once

#include "Sprite.h"
#include "ScriptResource.h"

#define SU_IF_COLOR "Color"
#define SU_IF_TF2D "Transform2D"

#define SU_IF_SPRITE "Sprite"
#define SU_IF_TXTSPRITE "TextSprite"

//Šî’ê‚ªImageSprite‚Å‚à‚¢‚¢‹C‚ª‚µ‚Ä‚é‚ñ‚¾‚æ‚Ë³’¼
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

    SSprite();
    virtual ~SSprite();
    void AddRef();
    void Release();

    virtual void Tick(double delta);
    virtual void Draw();

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

    void Draw() override;
};

//SpriteŒn‘S•”
void RegisterScriptSprite(asIScriptEngine *engine);