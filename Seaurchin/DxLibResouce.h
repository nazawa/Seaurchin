#pragma once


#include "Config.h"
#include "Debug.h"

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
};