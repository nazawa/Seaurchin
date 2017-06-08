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
    ExecutionManager *manager;
    std::unique_ptr<SusAnalyzer> analyzer;
    std::shared_ptr<MusicMetaInfo> metaInfo;
    std::map<std::string, SResource*> resources;
    SoundStream *bgmStream;

    // ‹È‚Ì“r’†‚Å•Ï‰»‚·‚é‚â‚Â‚ç
    std::vector<std::shared_ptr<SusDrawableNoteData>> data;
    std::vector<std::shared_ptr<SusDrawableNoteData>> seenData;
    uint32_t comboCount = 0;
    int seenObjects = 0;

    

    void CalculateNotes(double time, double duration, double preced);
    void DrawShortNotes(double time, double duration, double preced);
    void DrawLongNotes(double time, double duration, double preced);
    void DrawSlideNotes(double time, double duration, std::shared_ptr<SusDrawableNoteData>);
    void ProcessSound(double time, double duration, double preced);
    void ProcessScore(double time, double duration, double preced);

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