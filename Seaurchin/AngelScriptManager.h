#pragma once

#include <Windows.h>
#include <DxLib.h>
#include <angelscript.h>
#include <scriptbuilder\scriptbuilder.h>
#include <scriptarray\scriptarray.h>
#include <scriptmath\scriptmath.h>
#include <scriptmath\scriptmathcomplex.h>
#include <scriptstdstring\scriptstdstring.h>
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>

#include "Debug.h"

typedef std::function<bool(std::string, std::string, CScriptBuilder*)> IncludeCallback;

class AngelScript
{
private:
    asIScriptEngine *engine;
    asIScriptContext *sharedContext;
    CScriptBuilder builder;
    IncludeCallback includeFunc;

    void ScriptMessageCallback(const asSMessageInfo *message);

public:
    AngelScript();
    ~AngelScript();

    inline asIScriptEngine* GetEngine() { return engine; }
    inline asIScriptContext* GetContext() { return sharedContext; }

    void StartBuildModule(std::string name, IncludeCallback callback);
    void LoadFile(std::string filename);
    bool IncludeFile(std::string include, std::string from);
    bool FinishBuildModule();
    inline asIScriptModule* GetLastModule() { return builder.GetModule(); }
    bool CheckMetaData(asITypeInfo *type, std::string meta);

};