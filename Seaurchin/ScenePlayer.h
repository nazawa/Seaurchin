#pragma once

#include "Config.h"
#include "Scene.h"
#include "ScriptSprite.h"
#include "ScriptResource.h"
#include "SusAnalyzer.h"
#include "SoundManager.h"
#include "MusicsManager.h"

#define SU_IF_SCENE_PLAYER "ScenePlayer"

#define SU_LANE_X_MIN -400.0
#define SU_LANE_X_MAX 400.0
#define SU_LANE_Z_MIN 0.0
#define SU_LANE_Z_MAX 3000.0
#define SU_LANE_Y_GROUND 0.0
#define SU_LANE_Y_AIR 160.0
#define SU_LANE_ASPECT ((SU_LANE_Z_MAX - SU_LANE_Z_MIN) / (SU_LANE_X_MAX - SU_LANE_X_MIN))

// Scene‚Æ‚¢‚¤–¼‘O‚±‚»‚Â‚¢‚Ä‚é‚¯‚Ç‹““®‚Í•Ê•¨
// SceneManager‚É’Ç‰Á‚³‚ê‚È‚¢

enum NoteAttribute {
    Invisible = 0,
    Finished,
};

class ExecutionManager;
class ScenePlayer : public SSprite {
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
    std::unordered_map<std::shared_ptr<SusDrawableNoteData>, std::vector<std::tuple<double, double>>> curveData;
    uint32_t comboCount = 0;
    double currentTime = 0;
    double seenDuration = 0.750;
    double precedTime = 0.1;
    int seenObjects = 0;

    void CalculateNotes(double time, double duration, double preced);
    void CalculateCurves(std::shared_ptr<SusDrawableNoteData> note);
    void DrawShortNotes(std::shared_ptr<SusDrawableNoteData> note);
    void DrawAirNotes(std::shared_ptr<SusDrawableNoteData> note);
    void DrawHoldNotes(std::shared_ptr<SusDrawableNoteData> note);
    void DrawSlideNotes(std::shared_ptr<SusDrawableNoteData> note);
    std::tuple<double, double> DrawAirActionNotes(std::shared_ptr<SusDrawableNoteData> note);
    void Prepare3DDrawCall();

    void ProcessSound(double time, double duration, double preced);
    void ProcessScore(double time, double duration, double preced);

public:
    ScenePlayer(ExecutionManager *exm);
    ~ScenePlayer() override;

    void Initialize();
    void SetPlayerResource(const std::string &name, SResource *resource);
    void Draw() override;
    void Finalize();

    void Play();
    double GetPlayingTime();
    int GetSeenObjectsCount();
    void AdjustCamera(double cy, double cz, double ctz);
};

void RegisterPlayerScene(ExecutionManager *exm);