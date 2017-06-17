#include "ScriptExSprite.h"

SExSprite::SExSprite()
{}

SExSprite::~SExSprite()
{}

void SExSprite::AddRef()
{
    Reference++;
}

void SExSprite::Release()
{
    if (!--Reference) delete this;
}

void SExSprite::Tick(double delta)
{}

void SExSprite::Draw()
{}
