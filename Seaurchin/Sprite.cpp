#include "Sprite.h"

void SpriteCtorTransform2D(void *memory)
{
    new(memory) Transform2D();
}

void SpriteDtorTransform2D(void *memory)
{
    // nothing to do
}

Sprite* SpriteFactorySprite()
{
    return new Sprite();
}

Sprite::Sprite()
{
    ReferenceCount = 1;
    WriteDebugConsole("Sprite Constructed!\n");
}

Sprite::~Sprite()
{
    WriteDebugConsole("Sprite Destructed!\n");
}

void Sprite::Tick(float delta)
{
}

void Sprite::Draw()
{
}
