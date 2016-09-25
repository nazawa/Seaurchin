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
    void StartBuildModule(std::string name, IncludeCallback callback);
    
    //ファイル読み込み
    void LoadFile(std::string filename);
    
    //外から使わないで
    bool IncludeFile(std::string include, std::string from);
    
    //ビルドする
    bool FinishBuildModule();
    
    //FinishしたModuleを取得
    inline asIScriptModule* GetLastModule() { return builder.GetModule(); }

    //特定クラスにメタデータが付与されてるか
    bool CheckMetaData(asITypeInfo *type, std::string meta);

    //実装をチェック
    inline bool CheckImplementation(asITypeInfo *type, std::string name)
    {
        return type->Implements(engine->GetTypeInfoByName(name.c_str()));
    }
    
    //asITypeInfoからインスタンス作成 リファレンス無しなのでさっさとAddRefしろ
    asIScriptObject* InstantiateObject(asITypeInfo *type);
};