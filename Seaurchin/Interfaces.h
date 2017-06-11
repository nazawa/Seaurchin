#pragma once

#define SU_IF_KEY "Key"

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
T* asAssign(T * lhs, T* rhs)
{
    //rhs->AddRef();
    //lhs->Release();
    *lhs = *rhs;
    return lhs;
}

template <typename T>
void InterfacesRegisterSharedClass(asIScriptEngine *engine, std::string classname)
{
    engine->RegisterObjectType(classname.c_str(), sizeof(std::shared_ptr<T>), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK);
    engine->RegisterObjectBehaviour(classname.c_str(), asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(asConstruct<std::shared_ptr<T>>), asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectBehaviour(classname.c_str(), asBEHAVE_CONSTRUCT, ("void f(const " + classname + " & in)").c_str(), asFUNCTION(asCopyConstruct<std::shared_ptr<T>>), asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectBehaviour(classname.c_str(), asBEHAVE_DESTRUCT, "void f()", asFUNCTION(asDestroy<std::shared_ptr<T>>), asCALL_CDECL_OBJFIRST);
    engine->RegisterObjectMethod(classname.c_str(), (classname + " &opAssign(const " + classname + " &in)").c_str(), asFUNCTION(asAssign<std::shared_ptr<T>>), asCALL_CDECL_OBJFIRST);
}

class ExecutionManager;
void InterfacesRegisterEnum(ExecutionManager *exm);
void InterfacesRegisterGlobalFunction(ExecutionManager *exm);
void InterfacesRegisterSceneFunction(ExecutionManager *exm);