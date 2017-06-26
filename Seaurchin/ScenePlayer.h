#pragma once

#include "Config.h"
#include "Scene.h"
#include "ScriptSprite.h"
#include "ScriptResource.h"
#include "SusAnalyzer.h"
#include "ScoreProcessor.h"
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
enum JudgeType {
    ShortNormal = 0,
    ShortEx,
    SlideTap,
    Action,
};

enum PlayingState {
    
    ScoreNotLoaded,     //何も始まっていない
    BgmNotLoaded,       //譜面だけ読み込んだ
    ReadyToStart,       //読み込みが終わったので始められる
    ReadyCounting,      //BGM読み終わって前カウントしてる
    BgmPreceding,       //前カウント中だけどBGM始まってる
    OnlyScoreOngoing,   //譜面始まったけどBGMまだ
    BothOngoing,        //両方再生してる
    ScoreLasting,       //譜面残ってる
    BgmLasting,         //曲残ってる
};

class ExecutionManager;
class ScenePlayer : public SSprite {
    friend class ScoreProcessor;
    friend class AutoPlayerProcessor;
    friend class PlayableProcessor;

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
    ScoreProcessor *processor;
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
    SImage *imageTap, *imageExTap, *imageFlick, *imageHellTap;
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
    double CurrentTime = 0;
    double CurrentSoundTime = 0;
    double SeenDuration = 0.8;
    double PreloadingTime = 0.2;
    double BackingTime = 0.0;
    double NextMetronomeTime = 0.0;
    double SoundBufferingLatency = 0.030;   //TODO: 環境に若干寄り添う
    PlayingState State = PlayingState::ScoreNotLoaded;
    int seenObjects = 0;

    void AddSprite(SSprite *sprite);
    void LoadWorker();
    void RemoveSlideEffect();
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

    void SpawnJudgeEffect(std::shared_ptr<SusDrawableNoteData> target, JudgeType type);
    void SpawnSlideLoopEffect(std::shared_ptr<SusDrawableNoteData> target);
    void PlaySoundTap() { soundManager->PlayGlobal(soundTap->GetSample()); }
    void PlaySoundExTap() { soundManager->PlayGlobal(soundExTap->GetSample()); }
    void PlaySoundFlick() { soundManager->PlayGlobal(soundFlick->GetSample()); }
    void PlaySoundAir() { soundManager->PlayGlobal(soundAir->GetSample()); }
    void PlaySoundAirAction() { soundManager->PlayGlobal(soundAirAction->GetSample()); }
    void PlaySoundHold() { soundManager->PlayGlobal(soundHoldLoop->GetSample()); }
    void StopSoundHold() { soundManager->StopGlobal(soundHoldLoop->GetSample()); }
    void PlaySoundSlide() { soundManager->PlayGlobal(soundSlideLoop->GetSample()); }
    void StopSoundSlide() { soundManager->StopGlobal(soundSlideLoop->GetSample()); }

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
    void MovePositionBySecond(double sec);
    void MovePositionByMeasure(int meas);
};

void RegisterPlayerScene(ExecutionManager *exm);