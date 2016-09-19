#include "DxLibResouce.h"

using namespace std;

DxLibResource::DxLibResource()
{
}

DxLibResource::~DxLibResource()
{
}

// Font -----------------

VirtualFont::VirtualFont(string name, int size)
{
    fontName = name;
    this->size = size;
}

VirtualFont::~VirtualFont()
{
}

shared_ptr<VirtualFont> VirtualFont::Create(std::string name, int size)
{
    int h = CreateFontToHandle(name.c_str(), size, -1);
    if (!h) return shared_ptr<VirtualFont>();
    return shared_ptr<VirtualFont>(new VirtualFont(name, size));
}
