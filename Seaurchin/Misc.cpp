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

void ScriptSceneWarnOutOf(string type, asIScriptContext *ctx)
{
    const char *secn;
    int col, row;
    row = ctx->GetLineNumber(0, &col, &secn);
    ctx->GetEngine()->WriteMessage(secn, row, col, asEMsgType::asMSGTYPE_WARNING, ("You can call Yield Function only from " + type + "!").c_str());
}