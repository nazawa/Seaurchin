#pragma once

#include "Config.h"
#include "Debug.h"
#include "Setting.h"

#define BEGIN_DRAW_TRANSACTION(h) SetDrawScreen(h)
#define FINISH_DRAW_TRANSACTION SetDrawScreen(DX_SCREEN_BACK);

class DxLibResource
{
protected:
    int handle;

public:
    DxLibResource();
    ~DxLibResource();

    virtual inline int GetHandle() { return handle; }
};

class VirtualFont : public DxLibResource
{
protected:
    std::string fontName;
    int size;

private:
    VirtualFont(int h, std::string name, int size);

public:
    ~VirtualFont();

    static std::shared_ptr<VirtualFont> Create(std::string name, int size);

    inline std::string GetName() { return fontName; }
    inline int GetSize() { return size; }
    void DrawRaw(const std::string &str, double x, double y);
};

class Image : public DxLibResource
{
protected:
    int width = -1;
    int height = -1;
private:
    Image(int handle);

public:
    ~Image();
    static std::shared_ptr<Image> LoadFromFile(std::string fileName);
    static std::shared_ptr<Image> LoadFromMemory(void* buffer, int size);
};

class RenderTarget : public DxLibResource
{
private:
    RenderTarget(int w, int h);
public:
    ~RenderTarget();
    static std::shared_ptr<RenderTarget> Create(int w, int h);
};

class Font : public DxLibResource
{
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
        //イメージ名は64byteずつ
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

protected:
    std::vector<std::shared_ptr<Image>> images;
    std::vector<GlyphInfo*> chars; //大富豪プログラミング
    int size;

private:
    Font();
    static void SaveFontTexture(uint8_t *bitmap, int width, int height, std::string name);

public:

    ~Font();
    inline int GetSize() { return size; }
    int DrawRawUTF8(const std::string &str, double x, double y);
    static std::shared_ptr<Font> LoadFromFile(std::string fileName);
    static void CreateAndSave(std::string name, std::string save, int size, int texWidth, int texHeight);
};