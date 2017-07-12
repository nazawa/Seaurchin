#include "Interfaces.h"

#include "Config.h"
#include "Debug.h"
#include "ScriptScene.h"
#include "ExecutionManager.h"
#include "ScriptFunction.h"
#include "ScriptSprite.h"

using namespace std;



void InterfacesRegisterSceneFunction(ExecutionManager *exm)
{
	auto engine = exm->GetScriptInterfaceUnsafe()->GetEngine();

    engine->RegisterGlobalFunction("void YieldTime(double)", asFUNCTION(YieldTime), asCALL_CDECL);
    engine->RegisterGlobalFunction("void YieldFrame(int64)", asFUNCTION(YieldFrames), asCALL_CDECL);
}

void InterfacesRegisterGlobalFunction(ExecutionManager *exm)
{
	auto engine = exm->GetScriptInterfaceUnsafe()->GetEngine();

    engine->RegisterGlobalFunction("void WriteDebugConsole(const string &in)", asFUNCTION(WriteDebugConsoleU), asCALL_CDECL);
	engine->RegisterGlobalFunction(SU_IF_FONT "@ LoadSystemFont(const string & in)", asFUNCTION(LoadSystemFont), asCALL_CDECL);
    engine->RegisterGlobalFunction(SU_IF_IMAGE "@ LoadSystemImage(const string &in)", asFUNCTION(LoadSystemImage), asCALL_CDECL);
    engine->RegisterGlobalFunction("void CreateImageFont(const string &in, const string &in, int)", asFUNCTION(CreateImageFont), asCALL_CDECL);
}

void InterfacesRegisterEnum(ExecutionManager *exm)
{
	auto engine = exm->GetScriptInterfaceUnsafe()->GetEngine();

    engine->RegisterEnum(SU_IF_KEY);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_BACK", KEY_INPUT_BACK);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_TAB", KEY_INPUT_TAB);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_RETURN", KEY_INPUT_RETURN);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_LSHIFT", KEY_INPUT_LSHIFT);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_RSHIFT", KEY_INPUT_RSHIFT);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_LCONTROL", KEY_INPUT_LCONTROL);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_RCONTROL", KEY_INPUT_RCONTROL);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_ESCAPE", KEY_INPUT_ESCAPE);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_SPACE", KEY_INPUT_SPACE);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_PGUP", KEY_INPUT_PGUP);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_PGDN", KEY_INPUT_PGDN);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_END", KEY_INPUT_END);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_HOME", KEY_INPUT_HOME);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_LEFT", KEY_INPUT_LEFT);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_UP", KEY_INPUT_UP);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_RIGHT", KEY_INPUT_RIGHT);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_DOWN", KEY_INPUT_DOWN);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_INSERT", KEY_INPUT_INSERT);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_DELETE", KEY_INPUT_DELETE);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_MINUS", KEY_INPUT_MINUS);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_YEN", KEY_INPUT_YEN);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_PREVTRACK", KEY_INPUT_PREVTRACK);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_PERIOD", KEY_INPUT_PERIOD);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_SLASH", KEY_INPUT_SLASH);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_LALT", KEY_INPUT_LALT);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_RALT", KEY_INPUT_RALT);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_SCROLL", KEY_INPUT_SCROLL);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_SEMICOLON", KEY_INPUT_SEMICOLON);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_COLON", KEY_INPUT_COLON);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_LBRACKET", KEY_INPUT_LBRACKET);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_RBRACKET", KEY_INPUT_RBRACKET);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_AT", KEY_INPUT_AT);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_BACKSLASH", KEY_INPUT_BACKSLASH);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_COMMA", KEY_INPUT_COMMA);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_KANJI", KEY_INPUT_KANJI);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_CONVERT", KEY_INPUT_CONVERT);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_NOCONVERT", KEY_INPUT_NOCONVERT);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_KANA", KEY_INPUT_KANA);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_APPS", KEY_INPUT_APPS);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_CAPSLOCK", KEY_INPUT_CAPSLOCK);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_SYSRQ", KEY_INPUT_SYSRQ);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_PAUSE", KEY_INPUT_PAUSE);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_LWIN", KEY_INPUT_LWIN);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_RWIN", KEY_INPUT_RWIN);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_NUMLOCK", KEY_INPUT_NUMLOCK);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_NUMPAD0", KEY_INPUT_NUMPAD0);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_NUMPAD1", KEY_INPUT_NUMPAD1);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_NUMPAD2", KEY_INPUT_NUMPAD2);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_NUMPAD3", KEY_INPUT_NUMPAD3);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_NUMPAD4", KEY_INPUT_NUMPAD4);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_NUMPAD5", KEY_INPUT_NUMPAD5);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_NUMPAD6", KEY_INPUT_NUMPAD6);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_NUMPAD7", KEY_INPUT_NUMPAD7);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_NUMPAD8", KEY_INPUT_NUMPAD8);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_NUMPAD9", KEY_INPUT_NUMPAD9);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_MULTIPLY", KEY_INPUT_MULTIPLY);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_ADD", KEY_INPUT_ADD);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_SUBTRACT", KEY_INPUT_SUBTRACT);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_DECIMAL", KEY_INPUT_DECIMAL);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_DIVIDE", KEY_INPUT_DIVIDE);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_NUMPADENTER", KEY_INPUT_NUMPADENTER);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_F1", KEY_INPUT_F1);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_F2", KEY_INPUT_F2);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_F3", KEY_INPUT_F3);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_F4", KEY_INPUT_F4);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_F5", KEY_INPUT_F5);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_F6", KEY_INPUT_F6);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_F7", KEY_INPUT_F7);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_F8", KEY_INPUT_F8);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_F9", KEY_INPUT_F9);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_F10", KEY_INPUT_F10);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_F11", KEY_INPUT_F11);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_F12", KEY_INPUT_F12);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_A", KEY_INPUT_A);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_B", KEY_INPUT_B);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_C", KEY_INPUT_C);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_D", KEY_INPUT_D);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_E", KEY_INPUT_E);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_F", KEY_INPUT_F);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_G", KEY_INPUT_G);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_H", KEY_INPUT_H);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_I", KEY_INPUT_I);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_J", KEY_INPUT_J);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_K", KEY_INPUT_K);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_L", KEY_INPUT_L);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_M", KEY_INPUT_M);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_N", KEY_INPUT_N);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_O", KEY_INPUT_O);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_P", KEY_INPUT_P);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_Q", KEY_INPUT_Q);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_R", KEY_INPUT_R);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_S", KEY_INPUT_S);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_T", KEY_INPUT_T);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_U", KEY_INPUT_U);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_V", KEY_INPUT_V);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_W", KEY_INPUT_W);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_X", KEY_INPUT_X);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_Y", KEY_INPUT_Y);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_Z", KEY_INPUT_Z);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_0", KEY_INPUT_0);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_1", KEY_INPUT_1);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_2", KEY_INPUT_2);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_3", KEY_INPUT_3);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_4", KEY_INPUT_4);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_5", KEY_INPUT_5);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_6", KEY_INPUT_6);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_7", KEY_INPUT_7);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_8", KEY_INPUT_8);
    engine->RegisterEnumValue(SU_IF_KEY, "INPUT_9", KEY_INPUT_9);
}

// ‚»‚Ì‘¼“K“–‚ÈŠÖ”

void InterfacesExitApplication()
{
    auto hWnd = GetMainWindowHandle();
    PostMessage(hWnd, WM_CLOSE, 0, 0);
}