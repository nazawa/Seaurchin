#pragma once

#include "Config.h"
#include "Scene.h"
#include "ScriptSprite.h"
#include "ScriptResource.h"
#include "SusAnalyzer.h"
#include "SoundManager.h"
#include "MusicsManager.h"

#define SU_IF_SCENE_PLAYER "ScenePlayer"
#define SU_IF_PLAY_STATUS "PlayStatus"

#define SU_LANE_X_MIN -400.0
#define SU_LANE_X_MAX 400.0
#define SU_LANE_Z_MIN 0.0
#define SU_LANE_Z_MAX 3000.0
#define SU_LANE_Y_GROUND 0.0
#define SU_LANE_Y_AIR 240.0
#define SU_LANE_Y_AIRINDICATE 160.0
#define SU_LANE_ASPECT ((SU_LANE_Z_MAX - SU_LANE_Z_MIN) / (SU_LANE_X_MAX - SU_LANE_X_MIN))


// Sceneという名前こそついてるけど挙動は別物
// SceneManagerに追加されない

struct PlayStatus {
    uint32_t JusticeCritical = 0;
    uint32_t Justice = 0;
    uint32_t Attack = 0;
    uint32_t Miss = 0;

    uint32_t AllNotes = 0;
    uint32_t Combo = 0;
    double CurrentGauge = 0.0;
    double GaugeDefaultMax = 60000.0;

public:
    void GetGaugeValue(int &fulfilled, double &rest);
    uint32_t GetScore();
};

enum NoteAttribute {
    Invisible = 0,
    Finished,
};

enum JudgeType {
    ShortNormal = 0,
    ShortEx,
    SlideTap,
    Action,
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
    std::multiset<SSprite*, SSprite::Comparator> sprites;
    std::vector<SSprite*> spritesPending;

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
    SAnimatedImage *animeTap, *animeExTap, *animeSlideTap, *animeSlideLoop, *animeAirAction;
    STextSprite *textCombo;
    unsigned int slideLineColor = GetColor(0, 200, 255);
    unsigned int airActionLineColor = GetColor(0, 255, 32);

    //Slideの重みが若干違うらしいけどそのへん許してね
    PlayStatus Status;

    // 曲の途中で変化するやつら
    std::vector<std::shared_ptr<SusDrawableNoteData>> data;
    std::vector<std::shared_ptr<SusDrawableNoteData>> seenData;
    std::unordered_map<std::shared_ptr<SusDrawableNoteData>, SSprite*> SlideEffects;
    std::unordered_map<std::shared_ptr<SusDrawableNoteData>, std::vector<std::tuple<double, double>>> curveData;
    double currentTime = 0;
    double currentSoundTime = 0;
    double seenDuration = 0.8;
    double precedTime = 0.1;
    double BackingTime = 0.0;
    double NextMetronomeTime = 0.0;
    double SoundBufferingLatency = 0.030;   //TODO: 環境に若干寄り添う
    int BgmState = -1;  //-2: 読み込み終了 -1: 前カウント 0:プレイ中 1:曲終了
    int seenObjects = 0;
    bool isInHold = false, isInSlide = false, wasInHold = false, wasInSlide = false;

    void AddSprite(SSprite *sprite);
    void LoadWorker();
    void PrecalculateNotes();
    void IncrementCombo();
    void SpawnJudgeEffect(std::shared_ptr<SusDrawableNoteData> target, JudgeType type);
    void SpawnSlideLoopEffect(std::shared_ptr<SusDrawableNoteData> target);
    void UpdateSlideEffect();
    void CalculateNotes(double time, double duration, double preced);
    void CalculateCurves(std::shared_ptr<SusDrawableNoteData> note);
    void DrawShortNotes(std::shared_ptr<SusDrawableNoteData> note);
    void DrawAirNotes(std::shared_ptr<SusDrawableNoteData> note);
    void DrawHoldNotes(std::shared_ptr<SusDrawableNoteData> note);
    void DrawSlideNotes(std::shared_ptr<SusDrawableNoteData> note);
    std::tuple<double, double> DrawAirActionNotes(std::shared_ptr<SusDrawableNoteData> note);
    void DrawMeasureLines();
    void Prepare3DDrawCall();

    void ProcessSound();
    void ProcessScore(std::shared_ptr<SusDrawableNoteData> note);

public:
    ScenePlayer(ExecutionManager *exm);
    ~ScenePlayer() override;

    void AdjustCamera(double cy, double cz, double ctz);
    void SetPlayerResource(const std::string &name, SResource *resource);
    void Tick(double delta) override;
    void Draw() override;
    void Finalize();

    void Initialize();
    void Load();
    bool IsLoadCompleted();
    void Play();
    double GetPlayingTime();
    void GetPlayStatus(PlayStatus *status);
};

void RegisterPlayerScene(ExecutionManager *exm);