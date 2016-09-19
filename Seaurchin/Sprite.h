#pragma once

#include <Windows.h>
#include <DxLib.h>
#include <angelscript.h>

#include <string>
#include <functional>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <vector>
#include <memory>

#include "Config.h"
#include "Debug.h"

typedef struct
{
    double X = 0.0;
    double Y = 0.0;
    double Angle = 0.0;
    double OriginX = 0.0;
    double OriginY = 0.0;
    double ScaleX = 1.0;
    double ScaleY = 1.0;

} Transform2D;

/*
Sprite(AS互換、スマポ仕様)
全ての基底
*/
class Sprite
{
protected:
    int ReferenceCount;
public:
    Transform2D Transform;
    int ZIndex;

    Sprite();
    ~Sprite();

    void Tick(float delta);
    void Draw();
};

class ImageSprite : public Sprite
{

};

void SpriteCtorTransform2D(void *memory);
void SpriteDtorTransform2D(void *memory);
Sprite* SpriteFactorySprite();