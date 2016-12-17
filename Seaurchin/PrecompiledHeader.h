#pragma once

//Windows
#include <Windows.h>
#include <Shlwapi.h>
/*
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>
#include <xaudio2.h>
const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioClock = __uuidof(IAudioClock);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
*/

//C Runtime
#define _USE_MATH_DEFINES
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <cmath>

//C++ Standard
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <chrono>
#include <ios>
#include <map>
#include <utility>
#include <limits>
#include <unordered_map>
#include <forward_list>
#include <list>
#include <tuple>
#include <random>
#include <exception>

//Boost
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/regex.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/crc.hpp>
#include <boost/fusion/include/std_tuple.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/optional.hpp>

//Libraries
#include <DxLib.h>
//#include <EffekseerForDXLib.h>

#include <angelscript.h>
#include <scriptbuilder\scriptbuilder.h>
#include <scriptarray\scriptarray.h>
#include <scriptmath\scriptmath.h>
#include <scriptmath\scriptmathcomplex.h>
#include <scriptstdstring\scriptstdstring.h>
#include <scriptdictionary\scriptdictionary.h>
//#include "as_smart_ptr_wrapper.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <zlib.h>
#include <png.h>

#include <bass.h>
