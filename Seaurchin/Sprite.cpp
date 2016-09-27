#include "Sprite.h"

using namespace std;

void SpriteCtorTransform2D(void *memory)
{
    new(memory) Transform2D();
}

void SpriteDtorTransform2D(void *memory)
{
    // nothing to do
}

void SpriteCtorColorTint(void *memory)
{
    new(memory) ColorTint();
}

void SpriteDtorColorTint(void *memory)
{
    // nothing to do
}

// Sprite -----------------------------

shared_ptr<Sprite> SpriteFactorySprite(int number)
{
    return shared_ptr<Sprite>(new Sprite(number));
}

Sprite::Sprite()
{
    ReferenceCount = 0;
    WriteDebugConsole("Sprite Constructed!\n");
}

Sprite::Sprite(int number)
{

}

Sprite::~Sprite()
{

}

void Sprite::Tick(float delta)
{
}

void Sprite::Draw()
{
}

// ImageSprite -------------------------

shared_ptr<ImageSprite> SpriteFactoryImageSprite()
{
    return shared_ptr<ImageSprite>(new ImageSprite());
}

void ImageSprite::Draw()
{
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, Tint.A);
    SetDrawBright(Tint.R, Tint.G, Tint.B);
    DrawRotaGraph3F(
        Transform.X, Transform.Y,
        Transform.OriginX, Transform.OriginY,
        Transform.ScaleX, Transform.ScaleY,
        Transform.Angle,
        Source->GetHandle(), TRUE);
}

// TextSprite ----------------------------

void TextSprite::Draw()
{
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, Tint.A);
    SetDrawBright(Tint.R, Tint.G, Tint.B);
    DrawRotaGraph3F(
        Transform.X, Transform.Y,
        Transform.OriginX, Transform.OriginY,
        Transform.ScaleX, Transform.ScaleY,
        Transform.Angle,
        cache->GetHandle(), TRUE);
}

void TextSprite::Refresh()
{
    int w = Font->DrawRawUTF8(Text, INT_MIN, 0);
    cache = RenderTarget::Create(w, Font->GetSize());
    BEGIN_DRAW_TRANSACTION(cache->GetHandle());
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    SetDrawBright(255, 255, 255);
    Font->DrawRawUTF8(Text, 0, 0);
    FINISH_DRAW_TRANSACTION;
}

void TextSprite::SetText(const std::string & str)
{
    Text = str;
    Refresh();
}

std::shared_ptr<TextSprite> SpriteFactoryTextSprite()
{
    return std::shared_ptr<TextSprite>(new TextSprite());
}