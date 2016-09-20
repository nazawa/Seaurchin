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
    DrawRotaGraph3F(
        Transform.X, Transform.Y,
        Transform.OriginX, Transform.OriginY,
        Transform.ScaleX, Transform.ScaleY,
        Transform.Angle,
        Source->GetHandle(), TRUE);
}
