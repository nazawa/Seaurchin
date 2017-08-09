#pragma once

#include "Setting.h"
#include "AngelScriptManager.h"
#include "Scene.h"
#include "ScriptScene.h"
#include "EffectBuilder.h"
#include "SkinHolder.h"
#include "MusicsManager.h"
#include "SoundManager.h"
#include "ScenePlayer.h"
#include "Controller.h"

class ExecutionManager final {
private:
    std::shared_ptr<Setting> SharedSetting;
    std::unique_ptr<SettingItemManager> SettingManager;
    std::shared_ptr<AngelScript> ScriptInterface;
    std::vector<std::shared_ptr<Scene>> Scenes;
    std::vector<std::shared_ptr<Scene>> ScenesPending;
    std::shared_ptr<ControlState> SharedControlState;
    std::vector<std::wstring> SkinNames;
    std::unique_ptr<SkinHolder> Skin;
    std::unique_ptr<EffectBuilder> SuEffect;
    std::shared_ptr<std::mt19937> Random;
    std::shared_ptr<SoundManager> Sound;
    std::shared_ptr<MusicsManager> Musics;
    std::unordered_map<std::string, boost::any> optionalData;
    HIMC hImc;
    DWORD ImmConversion, ImmSentence;
    SSoundMixer *MixerBGM, *MixerSE;

public:
    ExecutionManager(std::shared_ptr<Setting> setting);

    void EnumerateSkins();
    void Tick(double delta);
    void Draw();
    void Initialize();
    void Shutdown();
    void AddScene(std::shared_ptr<Scene> scene);
    std::shared_ptr<ScriptScene> CreateSceneFromScriptType(asITypeInfo *type);
    std::shared_ptr<ScriptScene> CreateSceneFromScriptObject(asIScriptObject *obj);
    inline int GetSceneCount() { return Scenes.size(); }

    inline std::shared_ptr<MusicsManager> GetMusicsManager() { return Musics; }
    inline std::shared_ptr<ControlState> GetControlStateSafe() { return SharedControlState; }
    inline std::shared_ptr<Setting> GetSettingInstanceSafe() { return SharedSetting; }
    inline ControlState* GetControlStateUnsafe() { return SharedControlState.get(); }
    inline AngelScript* GetScriptInterfaceUnsafe() { return ScriptInterface.get(); }
    inline SoundManager* GetSoundManagerUnsafe() { return Sound.get(); }

    std::tuple<bool, LRESULT> CustomWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void ExecuteSkin();
    bool ExecuteSkin(const std::string &file);
    void ExecuteSystemMenu();
    void ReloadMusic();
    void Fire(const std::string &message);
    ScenePlayer *CreatePlayer();
    SSoundMixer *GetDefaultMixer(const std::string &name);
    SSettingItem *GetSettingItem(const std::string &group, const std::string &key);

    template<typename T>
    void SetData(const std::string &name, const T& data);
    template<typename T>
    T GetData(const std::string &name);
    bool ExistsData(const std::string &name) { return optionalData.find(name) != optionalData.end(); }

private:
    bool CheckSkinStructure(boost::filesystem::path name);
    void RegisterGlobalManagementFunction();
};

template<typename T>
void ExecutionManager::SetData(const std::string &name, const T & data)
{
    optionalData[name] = data;
}

template<typename T>
T ExecutionManager::GetData(const std::string &name)
{
    auto it = optionalData.find(name);
    return it == optionalData.end() ? T() : boost::any_cast<T>(it->second);
}
