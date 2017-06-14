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

// Sceneという名前こそついてるけど挙動は別物
// SceneManagerに追加されない

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
    SoundManager *soundManager;
    std::unique_ptr<SusAnalyzer> analyzer;
    std::map<std::string, SResource*> resources;
    SoundStream *bgmStream;
    bool isLoadCompleted = false;

    double cameraZ = -340, cameraY = 620, cameraTargetZ = 580; // スクショから計測
    double laneBufferX = 1024;
    double laneBufferY = laneBufferX * SU_LANE_ASPECT;
    double widthPerLane = laneBufferX / 16;
    double noteImageBlockX = 64;
    double noteImageBlockY = 64;
    double scaleNoteY = 2.0;
    double actualNoteScaleX = (widthPerLane / 2) / noteImageBlockX;
    double actualNoteScaleY = actualNoteScaleX * scaleNoteY;

    SSound *soundTap, *soundExTap, *soundFlick, *soundAir, *soundAirAction, *soundHoldLoop, *soundSlideLoop;
    SImage *imageTap, *imageExTap, *imageFlick;
    SImage *imageAirUp, *imageAirDown;
    SImage *imageHold, *imageHoldStrut;
    SImage *imageSlide, *imageSlideStrut;
    SImage *imageAirAction;
    SFont *fontCombo;
    STextSprite *textCombo;
    unsigned int slideLineColor = GetColor(0, 200, 255);
    unsigned int airActionLineColor = GetColor(0, 255, 32);

    //Slideの重みが若干違うらしいけどそのへん許してね
    int allNotesCount = 0;
    double gaugeDefaultMax = 60000;
    double currentGauge = 0;

    // 曲の途中で変化するやつら
    std::vector<std::shared_ptr<SusDrawableNoteData>> data;
    std::vector<std::shared_ptr<SusDrawableNoteData>> seenData;
    std::unordered_map<std::shared_ptr<SusDrawableNoteData>, std::vector<std::tuple<double, double>>> curveData;
    uint32_t comboCount = 0;
    double currentTime = 0;
    double seenDuration = 0.8;
    double precedTime = 0.1;
    int seenObjects = 0;
    bool isInHold = false, isInSlide = false, wasInHold = false, wasInSlide = false;

    void LoadWorker();
    void PrecalculateNotes();
    void IncrementCombo();
    void CalculateNotes(double time, double duration, double preced);
    void CalculateCurves(std::shared_ptr<SusDrawableNoteData> note);
    void DrawShortNotes(std::shared_ptr<SusDrawableNoteData> note);
    void DrawAirNotes(std::shared_ptr<SusDrawableNoteData> note);
    void DrawHoldNotes(std::shared_ptr<SusDrawableNoteData> note);
    void DrawSlideNotes(std::shared_ptr<SusDrawableNoteData> note);
    std::tuple<double, double> DrawAirActionNotes(std::shared_ptr<SusDrawableNoteData> note);
    void Prepare3DDrawCall();

    void ProcessSound(double time, double duration, double preced);
    void ProcessScore(std::shared_ptr<SusDrawableNoteData> note);

public:
    ScenePlayer(ExecutionManager *exm);
    ~ScenePlayer() override;

    void SetPlayerResource(const std::string &name, SResource *resource);
    void Tick(double delta) override;
    void Draw() override;
    void Finalize();

    void Initialize();
    void Load();
    bool IsLoadCompleted();
    void Play();
    double GetPlayingTime();
    int GetSeenObjectsCount();
    void AdjustCamera(double cy, double cz, double ctz);
    void GetCurrentGauge(int *fulfilled, double *current);
};

void RegisterPlayerScene(ExecutionManager *exm);