#pragma once

#include <Windows.h>
#include <DxLib.h>
#include <angelscript.h>
#include <vector>
#include <memory>
#include <algorithm>
#include "as_smart_ptr_wrapper.h"

#include "Config.h"
#include "Debug.h"
#include "Sprite.h"
#include "ScriptScene.h"

#define SU_IF_SCENE "Scene"
#define SU_IF_COSCENE "CoroutineScene"
#define SU_IF_TF2D "Transform2D"
#define SU_IF_SPRITE "Sprite"

//Helpers from as_smart_ptr_wrapper

template <typename T>
void asConstruct(void * address)
{
    new (address) T;
}

template <typename T>
void asDestroy(T * object)
{
    object->~T();
}

template <typename T>
void asCopyConstruct(void * address, T * other)
{
    new (address) T(*other);
}

template <typename T>
void asAssign(T * lhs, T* rhs)
{
    *lhs = *rhs;
}

template <typename T>
void InterfacesRegisterSharedClass(asIScriptEngine *engine, std::string classname)
{
    engine->RegisterObjectType(classname.c_str(), sizeof(std::shared_ptr<T>), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK);
    engine->RegisterObjectBehaviour(classname.c_str(), asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(construct<std::shared_ptr<T>>), asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectBehaviour(classname.c_str(), asBEHAVE_CONSTRUCT, ("void f(const " + engine + " & in)").c_str(), asFUNCTION(copy_construct<std::shared_ptr<T>>), asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectBehaviour(classname.c_str(), asBEHAVE_DESTRUCT, "void f()", asFUNCTION(destroy<std::shared_ptr<T>>), asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod(classname.c_str(), (classname + " &opAssign(const Base &in)").c_str(), asFUNCTION(assign<std::shared_ptr<T>>), asCALL_CDECL_OBJFIRST);
}

void InterfacesRegisterScene(asIScriptEngine *engine);
void InterfacesRegisterSprite(asIScriptEngine *engine);
void InterfacesRegisterGlobalFunction(asIScriptEngine *engine);
void InterfaceRegisterSceneFunction(asIScriptEngine *engine);