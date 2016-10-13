#include "Misc.h"
#include "ScriptScene.h"

using namespace std;

string ConvertUTF8ToShiftJis(string utf8str)
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

wstring ConvertUTF8ToUnicode(string utf8str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, nullptr, 0);
    wchar_t *buffer = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, utf8str.c_str(), -1, buffer, len);
    wstring ret = buffer;
    delete[] buffer;
    return ret;
}

void ScriptSceneWarnOutOf(string type, asIScriptContext *ctx)
{
    const char *secn;
    int col, row;
    row = ctx->GetLineNumber(0, &col, &secn);
    ctx->GetEngine()->WriteMessage(secn, row, col, asEMsgType::asMSGTYPE_WARNING, ("You can call Yield Function only from " + type + "!").c_str());
}

double ToDouble(const char *str)
{
    double result = 0.0, base = 1.0;
    int sign = 1;
    const char *it = str;
    unsigned char ch = '0';

    it = *it == '-' ? (sign = -sign, ++it) : it;
    while (((unsigned char)(ch = *(it++) - '0')) <= 9) result = result * 10 + ch;
    if (*(--it) == '.') while (((unsigned char)(ch = *(++it) - '0')) <= 9) result += (base *= 0.1) * ch;
    return sign * result;
}