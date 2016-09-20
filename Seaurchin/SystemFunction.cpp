#include "SystemFunction.h"

using namespace std;
using namespace boost::filesystem;

static string ConvertUTF8ToShiftJis(string utf8str);

static string ConvertUTF8ToShiftJis(string utf8str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, nullptr, 0);
    wchar_t *buffer = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, buffer, len);
    int sjis = WideCharToMultiByte(CP_ACP, 0, buffer, -1, nullptr, 0, nullptr, nullptr);
    char *sbuffer = new char[sjis];
    WideCharToMultiByte(CP_ACP, 0, buffer, -1, sbuffer, sjis, nullptr, nullptr);
    string ret = sbuffer;
    delete[] buffer;
    delete[] sbuffer;
    return ret;
}

shared_ptr<Image> LoadSystemImage(const string & file)
{
    path p = SettingGetRootDirectory() / SU_DATA_DIR / SU_IMAGE_DIR / ConvertUTF8ToShiftJis(file);
    return Image::LoadFromFile(p.string());
}

shared_ptr<VirtualFont> CreateVirtualFont(const string & name, int size)
{
    return VirtualFont::Create(name, size);
}

void DrawRawString(shared_ptr<VirtualFont> font, const string &str, double x, double y)
{
    font->DrawRaw(ConvertUTF8ToShiftJis(str), x, y);
}