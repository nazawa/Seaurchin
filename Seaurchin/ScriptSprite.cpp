#include "ScriptSprite.h"
#include "ScriptSpriteManager.h"
#include "Misc.h"

using namespace std;

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
    mover = new ScriptSpriteMover(this);
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

void SSprite::AddMove(const string & move)
{
    mover->AddMove(move);
}

void SSprite::Apply(const string & dict)
{
    using namespace boost::algorithm;
    constexpr auto hash = &crc_ccitt::checksum;
    string list = dict;
    list.erase(remove(list.begin(), list.end(), ' '), list.end());
    vector<string> params;
    split(params, list, is_any_of(","));
    
    vector<string> pr;
    for (auto& p : params)
    {
        pr.clear();
        split(pr, p, is_any_of(":"));
        if (pr.size() != 2) continue;
        switch (hash(pr[0].c_str()))
        {
        case hash("x"):
            Transform.X = atof(pr[1].c_str());
            break;
        case hash("y"):
            Transform.Y = atof(pr[1].c_str());
            break;
        case hash("origX"):
            Transform.OriginX = atof(pr[1].c_str());
            break;
        case hash("origY"):
            Transform.OriginY = atof(pr[1].c_str());
            break;
        case hash("scaleX"):
            Transform.ScaleX = atof(pr[1].c_str());
            break;
        case hash("scaleY"):
            Transform.ScaleY = atof(pr[1].c_str());
            break;
        case hash("angle"):
            Transform.Angle = atof(pr[1].c_str());
            break;
        case hash("alpha"):
            Color.A = (unsigned char)(atof(pr[1].c_str()) * 255.0);
            break;
        case hash("r"):
            Color.R = (unsigned char)atoi(pr[1].c_str());
            break;
        case hash("g"):
            Color.G = (unsigned char)atoi(pr[1].c_str());
            break;
        case hash("b"):
            Color.B = (unsigned char)atoi(pr[1].c_str());
            break;
        }
    }
}

void SSprite::Apply(const CScriptDictionary & dict)
{
    constexpr auto hash = &crc_ccitt::checksum;
    ostringstream aps;

    auto i = dict.begin();
    while (i != dict.end())
    {
        auto key = i.GetKey();
        aps << key << ":";
        double dv = 0;
        aps << i.GetValue(dv) << ", ";
        i++;
    }

    Apply(aps.str());
}

void SSprite::Tick(double delta)
{
    mover->Tick(delta);
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

SSprite * SSprite::Clone()
{
    //ƒRƒsƒRƒ“‚Å—Ç‚­‚È‚¢‚©‚±‚ê
    auto clone = new SSprite();
    if (Image)
    {
        Image->AddRef();
        clone->set_Image(Image);
    }
    clone->Color = Color;
    clone->Transform = Transform;
    clone->ZIndex = ZIndex;
    clone->IsDead = IsDead;
    clone->HasAlpha = true;
    clone->AddRef();
    return clone;
}

SSprite * SSprite::Factory()
{
    auto result = new SSprite();
    result->AddRef();
    return result;
}

SSprite * SSprite::Factory(SImage * img)
{
    auto result = new SSprite();
    result->set_Image(img);
    result->AddRef();
    return result;
}

ColorTint GetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return ColorTint{ a, r, g, b };
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
    engine->RegisterObjectBehaviour(SU_IF_SPRITE, asBEHAVE_FACTORY, SU_IF_SPRITE "@ f()", asFUNCTIONPR(SSprite::Factory, (), SSprite*), asCALL_CDECL);
    engine->RegisterObjectBehaviour(SU_IF_SPRITE, asBEHAVE_FACTORY, SU_IF_SPRITE "@ f(" SU_IF_IMAGE "@)", asFUNCTIONPR(SSprite::Factory, (SImage*), SSprite*), asCALL_CDECL);
    engine->RegisterObjectMethod(SU_IF_SPRITE, SU_IF_SPRITE "@ Clone()", asMETHOD(SSprite, Clone), asCALL_THISCALL);
}

// Shape -----------------

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

SShape * SShape::Factory()
{
    auto result = new SShape();
    result->AddRef();
    return result;
}

void SShape::RegisterType(asIScriptEngine * engine)
{
    RegisterSpriteBasic<SShape>(engine, SU_IF_SHAPE);
    engine->RegisterObjectBehaviour(SU_IF_SHAPE, asBEHAVE_FACTORY, SU_IF_SHAPE "@ f()", asFUNCTIONPR(SShape::Factory, (), SShape*), asCALL_CDECL);
    engine->RegisterObjectMethod(SU_IF_SPRITE, SU_IF_SHAPE "@ opCast()", asFUNCTION((CastReferenceType<SSprite, SShape>)), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod(SU_IF_SHAPE, SU_IF_SPRITE "@ opImplCast()", asFUNCTION((CastReferenceType<SShape, SSprite>)), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectProperty(SU_IF_SHAPE, "double Width", asOFFSET(SShape, Width));
    engine->RegisterObjectProperty(SU_IF_SHAPE, "double Height", asOFFSET(SShape, Height));
    engine->RegisterObjectProperty(SU_IF_SHAPE, SU_IF_SHAPETYPE " Type", asOFFSET(SShape, Type));
}

// TextSprite -----------

void STextSprite::Refresh()
{
    if (!Font) return;
    if (Target) delete Target;
    wstring cs = ConvertUTF8ToUnicode(Text);
    auto size = Font->RenderRaw(nullptr, cs);
    Target = new SRenderTarget((int)get<0>(size), (int)get<1>(size));
    Font->RenderRaw(Target, cs);
}

void STextSprite::set_Font(SFont * font)
{
    Font = font;
    Refresh();
}

void STextSprite::set_Text(const std::string & txt)
{
    Text = txt;
    Refresh();
}

STextSprite::~STextSprite()
{
    if (Font) Font->Release();
    if (Target) delete Target;
}

void STextSprite::Draw()
{
    if (!Target) return;
    SetDrawBright(Color.R, Color.G, Color.B);
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, Color.A);
    DrawRotaGraph3F(
        Transform.X, Transform.Y,
        Transform.OriginX, Transform.OriginY,
        Transform.ScaleX, Transform.ScaleY,
        Transform.Angle, Target->GetHandle(),
        TRUE, FALSE);
}

STextSprite * STextSprite::Factory()
{
    auto result = new STextSprite();
    result->AddRef();
    return result;
}

STextSprite * STextSprite::Factory(SFont * img, const std::string & str)
{
    auto result = new STextSprite();
    result->set_Font(img);
    result->set_Text(str);
    result->AddRef();
    return result;
}

void STextSprite::RegisterType(asIScriptEngine * engine)
{
    RegisterSpriteBasic<STextSprite>(engine, SU_IF_TXTSPRITE);
    engine->RegisterObjectBehaviour(SU_IF_TXTSPRITE, asBEHAVE_FACTORY, SU_IF_TXTSPRITE "@ f()", asFUNCTIONPR(STextSprite::Factory, (), STextSprite*), asCALL_CDECL);
    engine->RegisterObjectBehaviour(SU_IF_TXTSPRITE, asBEHAVE_FACTORY, SU_IF_TXTSPRITE "@ f(" SU_IF_FONT "@, const string &in)", asFUNCTIONPR(STextSprite::Factory, (SFont*, const string&), STextSprite*), asCALL_CDECL);
    engine->RegisterObjectMethod(SU_IF_SPRITE, SU_IF_TXTSPRITE "@ opCast()", asFUNCTION((CastReferenceType<SSprite, STextSprite>)), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod(SU_IF_TXTSPRITE, SU_IF_SPRITE "@ opImplCast()", asFUNCTION((CastReferenceType<STextSprite, SSprite>)), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod(SU_IF_TXTSPRITE, "void SetFont(" SU_IF_FONT "@)", asMETHOD(STextSprite, set_Font), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_TXTSPRITE, "void SetText(const string &in)", asMETHOD(STextSprite, set_Text), asCALL_THISCALL);
}

// ˆê”Ê

void RegisterScriptSprite(asIScriptEngine * engine)
{
    SSprite::RegisterType(engine);
    SShape::RegisterType(engine);
    STextSprite::RegisterType(engine);
}