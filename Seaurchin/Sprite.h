#pragma once


#include "DxLibResouce.h"
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
    Sprite(int number);
    ~Sprite();

    virtual void Tick(float delta);
    virtual void Draw();
};

class ImageSprite : public Sprite
{
public:
    std::shared_ptr<Image> Source;
    void Draw() override;
};

class TextSprite : public Sprite
{
private:
    int handle;
    std::shared_ptr<RenderTarget> cache;

public:
    std::shared_ptr<Font> Font;
    std::string Text;           //UTF-8状態

    void Draw() override;
    void Refresh();
    void SetText(const std::string& str);
};

class Shape : public Sprite
{
public:
    double Width;
    double Height;
};

void SpriteCtorTransform2D(void *memory);
void SpriteDtorTransform2D(void *memory);
std::shared_ptr<Sprite> SpriteFactorySprite(int number);
std::shared_ptr<ImageSprite> SpriteFactoryImageSprite();
std::shared_ptr<TextSprite> SpriteFactoryTextSprite();