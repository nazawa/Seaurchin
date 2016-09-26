#pragma once


typedef struct
{
    uint16_t texture;
    uint16_t width;
    uint16_t height;
    uint16_t x;
    uint16_t y;
    int16_t bearX;
    int16_t bearY;
    uint16_t wholeAdvance;
    wchar_t letter;
} GlyphInfo;

typedef struct
{
    //ƒCƒ[ƒW–¼‚Í64byte‚¸‚Â
    uint16_t ImageCount;
    uint32_t GlyphCount;
    int Size;
} FontDataHeader;

class RectPacker final
{
private:
    int width;
    int height;
    int row;
    int cursorX = 0;
    int cursorY = 0;

public:
    typedef struct
    {
        int x;
        int y;
        int width;
        int height;
    } Rect;

    void Init(int w, int h, int rowh);

    Rect Insert(int w, int h);
};
