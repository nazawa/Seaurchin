#pragma once

#include "Config.h"
#include "Scene.h"
#include "ScriptResource.h"
#include "SusAnalyzer.h"
#include "SoundManager.h"
#include "MusicsManager.h"

#define SU_IF_SCENE_PLAYER "ScenePlayer"

// Scene‚Æ‚¢‚¤–¼‘O‚±‚»‚Â‚¢‚Ä‚é‚¯‚Ç‹““®‚Í•Ê•¨
// SceneManager‚É’Ç‰Á‚³‚ê‚È‚¢

enum NoteAttribute {
    Invisible = 0,
    Finished,
};

class ExecutionManager;
class ScenePlayer {
protected:
    int reference = 0;
    int hGroundBuffer;
    int hAirBuffer;
    std::vector<std::shared_ptr<SusDrawableNoteData>> data;
    std::map<std::string, SResource*> resources;
    SoundStream *bgmStream;
    int seenObjects = 0;

    ExecutionManager *manager;
    std::unique_ptr<SusAnalyzer> analyzer;
    std::shared_ptr<MusicMetaInfo> metaInfo;

public:
    ScenePlayer(ExecutionManager *exm);

    void AddRef();
    void Release();

    void Initialize();
    void SetPlayerResource(const std::string &name, SResource *resource);
    void Draw();
    void Finalize();

    void Play();
    double GetPlayingTime();
    int GetSeenObjectsCount();
};

void RegisterPlayerScene(ExecutionManager *exm);