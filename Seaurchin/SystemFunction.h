#pragma once

#include "Setting.h"
#include "DxLibResouce.h"

std::shared_ptr<Image> LoadSystemImage(const std::string & file);
std::shared_ptr<VirtualFont> CreateVirtualFont(const std::string & name, int size);
void DrawRawString(std::shared_ptr<VirtualFont> font, const std::string &str, double x, double y);