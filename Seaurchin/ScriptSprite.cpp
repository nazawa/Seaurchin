#include "ScriptSprite.h"

void SSprite::set_Image(SImage * img)
{
    if (Image) Image->Release();
    Image = img;
    
    //Image->AddRef();
}

const SImage * SSprite::get_Image()
{
    if (Image) Image->AddRef();
    return Image;
}

SSprite::SSprite()
{
    //ZIndex = 0;
    Color = Colors::White;
}

SSprite::~SSprite()
{
    WriteDebugConsole("Destructing ScriptSprite\n");
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
    
    if (!Image) return;
    SetDrawBright(Color.R, Color.G, Color.B);
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, Color.A);
    DrawRotaGraph3F(
        Transform.X, Transform.Y,
        Transform.OriginX, Transform.OriginY,
        Transform.ScaleX, Transform.ScaleY,
        Transform.Angle, Image->GetHandle(),
        HasAlpha ? TRUE : FALSE, FALSE);
        
}

SSprite * SSprite::Factory()
{
    auto result = new SSprite();
    result->AddRef();
    return result;
}


void SShape::Draw()
{
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, Color.A);
    switch (Type)
    {
    case SShapeType::Pixel:
        DrawPixel(Transform.X, Transform.Y, GetColor(Color.R, Color.G, Color.B));
        break;
    case SShapeType::Box:
        DrawBoxAA(
            Transform.X - Width / 2, Transform.Y - Height / 2,
            Transform.X + Width / 2, Transform.Y + Height / 2,
            GetColor(Color.R, Color.G, Color.B), FALSE);
        break;
    case SShapeType::BoxFill:
        DrawBoxAA(
            Transform.X - Width / 2, Transform.Y - Height / 2,
            Transform.X + Width / 2, Transform.Y + Height / 2,
            GetColor(Color.R, Color.G, Color.B), TRUE);
        break;
    case SShapeType::Oval:
        DrawOvalAA(
            Transform.X, Transform.Y,
            Width / 2, Height / 2,
            256, GetColor(Color.R, Color.G, Color.B), FALSE);
        break;
    case SShapeType::OvalFill:
        DrawOvalAA(
            Transform.X, Transform.Y,
            Width / 2, Height / 2,
            256, GetColor(Color.R, Color.G, Color.B), TRUE);
        break;
    }
}

SSprite * SShape::Factory()
{
    auto result = new SShape();
    result->AddRef();
    return result;
}

void SShape::RegisterType(asIScriptEngine * engine)
{
    RegisterSpriteBasic<SShape>(engine, SU_IF_SHAPE);
    engine->RegisterObjectMethod(SU_IF_SPRITE, SU_IF_SHAPE "@ opCast()", asFUNCTION((CastReferenceType<SShape, SSprite>)), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod(SU_IF_SHAPE, SU_IF_SPRITE "@ opImplCast()", asFUNCTION((CastReferenceType<SSprite, SShape>)), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectProperty(SU_IF_SHAPE, "double Width", asOFFSET(SShape, Width));
    engine->RegisterObjectProperty(SU_IF_SHAPE, "double Height", asOFFSET(SShape, Height));
    engine->RegisterObjectProperty(SU_IF_SHAPE, SU_IF_SHAPETYPE " Type", asOFFSET(SShape, Type));
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
    SShape::RegisterType(engine);
}