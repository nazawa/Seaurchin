#pragma once

#include "Font.h"
#include "Misc.h"

#define SU_IF_IMAGE "Image"
#define SU_IF_FONT "Font"
#define SU_IF_RENDER "RenderTarget"
#define SU_IF_SOUND "Sound"

class SResource
{
protected:
    int Reference = 0;
    int Handle = 0;
public:
    SResource();
    virtual ~SResource();
    void AddRef();
    void Release();

    inline int GetHandle() { return Handle; }
};

class SImage : public SResource
{
protected:
    int Width = 0;
    int Height = 0;

    void ObtainSize();
public:
    SImage(int ih);
    ~SImage() override;

    int get_Width();
    int get_Height();

    static SImage* CreateBlankImage();
    static SImage* CreateLoadedImageFromFile(const std::string &file);
    static SImage* CreateLoadedImageFromMemory(void *buffer, size_t size);
};

class SRenderTarget : public SImage
{
public:
    SRenderTarget(int w, int h);
    ~SRenderTarget();
};

class SFont : public SResource
{
protected:
    int Size = 0;
    std::vector<GlyphInfo*> Chars;
    std::vector<SImage*> Images;

public:
    SFont();
    ~SFont();

    inline int get_Size() { return Size; }

    static SFont* CreateBlankFont();
    static SFont* CreateLoadedFontFromFile(const std::string &file);
};

class SSound : public SResource
{

};

void RegisterScriptResource(asIScriptEngine *engine);