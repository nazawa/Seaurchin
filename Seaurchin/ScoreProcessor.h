#pragma once

#include "SusAnalyzer.h"

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

class ScenePlayer;
class ScoreProcessor {
public:
    virtual void Reset() = 0;
    virtual void Update(std::vector<std::shared_ptr<SusDrawableNoteData>> &notes) = 0;
    virtual void MovePosition(double relative) = 0;
    virtual void Draw() = 0;
    virtual PlayStatus *GetPlayStatus() = 0;
};

class AutoPlayerProcessor : public ScoreProcessor {
protected:
    static std::vector<std::shared_ptr<SusDrawableNoteData>> DefaultDataValue;

    ScenePlayer *Player;
    PlayStatus Status;
    std::vector<std::shared_ptr<SusDrawableNoteData>> &data = DefaultDataValue;
    bool isInHold = false, isInSlide = false, wasInHold = false, wasInSlide = false;

    void ProcessScore(std::shared_ptr<SusDrawableNoteData>notes);
    void IncrementCombo();

public:
    AutoPlayerProcessor(ScenePlayer *player);

    void Reset() override;
    void Update(std::vector<std::shared_ptr<SusDrawableNoteData>> &notes) override;
    void MovePosition(double relative) override;
    void Draw() override;
    PlayStatus *GetPlayStatus() override;
};