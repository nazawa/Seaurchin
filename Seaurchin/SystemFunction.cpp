#include "SystemFunction.h"

using namespace std;
using namespace boost::filesystem;

static string ConvertUTF8ToShiftJis(string utf8str);
static int CALLBACK FontEnumerationProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, DWORD FontType, LPARAM lParam);

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

static int CALLBACK FontEnumerationProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, DWORD FontType, LPARAM lParam)
{
    return 0;
}

shared_ptr<Image> LoadSystemImage(const string & file)
{
    path p = SettingGetRootDirectory() / SU_DATA_DIR / SU_IMAGE_DIR / ConvertUTF8ToShiftJis(file);
    return Image::LoadFromFile(p.string());
}

std::shared_ptr<Font> LoadSystemFont(const std::string & file)
{
    EnumerateInstalledFonts();
    path p = SettingGetRootDirectory() / SU_DATA_DIR / SU_FONT_DIR / (ConvertUTF8ToShiftJis(file) + ".sif");
    return Font::LoadFromFile(p.string());
}

shared_ptr<VirtualFont> CreateVirtualFont(const string & name, int size)
{
    return VirtualFont::Create(name, size);
}

void CreateImageFont(const std::string & fileName, const std::string & saveName, int size)
{
    Font::CreateAndSave(fileName, saveName, size, 1024, 1024);
}

void DrawRawString(shared_ptr<VirtualFont> font, const string &str, double x, double y)
{
    font->DrawRaw(ConvertUTF8ToShiftJis(str), x, y);
}

void EnumerateInstalledFonts()
{
    //HKEY_LOCAL_MACHINE\Software\Microsoft\Windows NT\CurrentVersion\Fonts
    HDC hdc = GetDC(GetMainWindowHandle());
    LOGFONT logfont;
    logfont.lfCharSet = DEFAULT_CHARSET;
    memcpy_s(logfont.lfFaceName, sizeof(logfont.lfFaceName), "", 1);
    logfont.lfPitchAndFamily = 0;
    EnumFontFamiliesEx(hdc, &logfont, (FONTENUMPROC)FontEnumerationProc, (LPARAM)nullptr, 0);
}