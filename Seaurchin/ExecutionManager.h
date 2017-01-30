#pragma once

#include "Setting.h"
#include "AngelScriptManager.h"
#include "DxLibResource.h"
#include "Scene.h"
#include "ScriptScene.h"
#include "EffectBuilder.h"
#include "SkinHolder.h"
#include "MusicsManager.h"
#include "SoundManager.h"

class ExecutionManager final
{
private:
    std::shared_ptr<Setting> SharedSetting;
    std::shared_ptr<AngelScript> ScriptInterface;
    std::vector<std::shared_ptr<Scene>> Scenes;
    std::vector<std::shared_ptr<Scene>> ScenesPending;
    std::shared_ptr<KeyState> SharedKeyState;
    std::vector<std::string> SkinNames;
    std::unique_ptr<SkinHolder> Skin;
    std::unique_ptr<EffectBuilder> SuEffect;
    std::shared_ptr<std::mt19937> Random;
	std::shared_ptr<SoundManager> Sound;
    std::shared_ptr<MusicsManager> Musics;


public:
    ExecutionManager(std::shared_ptr<Setting> setting);

    void EnumerateSkins();
    void Tick(double delta);
    void Draw();
    void AddScene(std::shared_ptr<Scene> scene);
    std::shared_ptr<ScriptScene> CreateSceneFromScriptType(asITypeInfo *type);
    std::shared_ptr<ScriptScene> CreateSceneFromScriptObject(asIScriptObject *obj);
    inline int GetSceneCount() { return Scenes.size(); }
    inline KeyState* GetKeyState() { return SharedKeyState.get(); }
    inline AngelScript* GetScriptInterface() { return ScriptInterface.get(); }
	inline SoundManager* GetSoundManager() { return Sound.get(); }

    std::tuple<bool, LRESULT> CustomWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void ExecuteSkin();
    void ExecuteSkin(const std::string &file);
    void ExecuteSystemMenu();
    void ReloadMusic();
    MusicSelectionCursor *CreateCursor();

private:
    bool CheckSkinStructure(boost::filesystem::path name);
    void UpdateKeyState();
    void RegisterGlobalManagementFunction();
};

