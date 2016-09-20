#include "DxLibResouce.h"

using namespace std;

DxLibResource::DxLibResource()
{
}

DxLibResource::~DxLibResource()
{
}

// Font -----------------

VirtualFont::VirtualFont(int h, string name, int size)
{
    fontName = name;
    this->size = size;
    handle = h;
}

VirtualFont::~VirtualFont()
{
    DeleteFontToHandle(handle);
}

shared_ptr<VirtualFont> VirtualFont::Create(std::string name, int size)
{
    int h = CreateFontToHandle(name.c_str(), size, -1, DX_FONTTYPE_ANTIALIASING_4X4);
    return shared_ptr<VirtualFont>(new VirtualFont(h, name, size));
}

void VirtualFont::DrawRaw(const string & str, double x, double y)
{
    DrawStringFToHandle(x, y, str.c_str(), GetColor(255, 255, 255), handle);
}

Image::Image(int handle)
{
    this->handle = handle;
}

Image::~Image()
{
    DeleteGraph(handle);
    ASSERT_CALL;
}

std::shared_ptr<Image> Image::LoadFromFile(std::string fileName)
{
    return std::shared_ptr<Image>(new Image(LoadGraph(fileName.c_str())));
}
