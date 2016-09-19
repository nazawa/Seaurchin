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
    VirtualFont(std::string name, int size);

public:
    ~VirtualFont();

    static std::shared_ptr<VirtualFont> Create(std::string name, int size);

    inline std::string GetName() { return fontName; }
    inline int GetSize() { return size; }
};