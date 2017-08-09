#pragma once

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

    //新しくModuleする
    void StartBuildModule(const std::string &name, IncludeCallback callback);

    inline asIScriptModule* GetExistModule(std::string name) { return engine->GetModule(name.c_str()); }
    
    //ファイル読み込み
    void LoadFile(const std::wstring &filename);
    
    //外から使わないで
    bool IncludeFile(const std::wstring &include, const std::wstring &from);
    
    //ビルドする
    bool FinishBuildModule();
    
    //FinishしたModuleを取得
    inline asIScriptModule* GetLastModule() { return builder.GetModule(); }

    //特定クラスにメタデータが付与されてるか
    bool CheckMetaData(asITypeInfo *type, std::string meta);
    
    //特定グロ関に(ry
    bool CheckMetaData(asIScriptFunction *type, std::string meta);


    //実装をチェック
    inline bool CheckImplementation(asITypeInfo *type, std::string name)
    {
        return type->Implements(engine->GetTypeInfoByName(name.c_str()));
    }
    
    //asITypeInfoからインスタンス作成 リファレンス無しなのでさっさとAddRefしろ
    asIScriptObject* InstantiateObject(asITypeInfo *type);
};