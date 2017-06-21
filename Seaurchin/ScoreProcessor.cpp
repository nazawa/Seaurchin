#include "ScoreProcessor.h"
#include "ScenePlayer.h"

using namespace std;

// PlayStatus -------------------------------------------------

void PlayStatus::GetGaugeValue(int &fulfilled, double &rest)
{
    fulfilled = 0;
    rest = 0;
    double calc = round(CurrentGauge);
    double currentMax = 12000;
    while (calc >= currentMax) {
        fulfilled += 1;
        calc -= currentMax;
        currentMax += 2000;
    }
    rest = calc / currentMax;
}

uint32_t PlayStatus::GetScore()
{
    double result = 0;
    double base = 1000000.0 / AllNotes;
    result += JusticeCritical * base * 1.01;
    result += Justice * base * 1.00;
    result += Attack * base * 0.50;
    return (uint32_t)round(result);
}

// ScoreProcessor-s -------------------------------------------

std::vector<std::shared_ptr<SusDrawableNoteData>> AutoPlayerProcessor::DefaultDataValue;

AutoPlayerProcessor::AutoPlayerProcessor(ScenePlayer *player)
{
    Player = player;
}

void AutoPlayerProcessor::Reset()
{
    data = Player->data;
    Status.AllNotes = 0;
    for (auto &note : data) {
        auto type = note->Type.to_ulong();
        if (type & 0b0000000011100000) {
            if (!note->Type.test(SusNoteType::AirAction)) Status.AllNotes++;
            for (auto &ex : note->ExtraData)
                if (
                    ex->Type.test(SusNoteType::End)
                    || ex->Type.test(SusNoteType::Step)
                    || ex->Type.test(SusNoteType::ExTap))
                    Status.AllNotes++;
        } else if (type & 0b0000000000011110) {
            Status.AllNotes++;
        }
    }
}

void AutoPlayerProcessor::Update(vector<shared_ptr<SusDrawableNoteData>> &notes)
{
    bool SlideCheck = false;
    bool HoldCheck = false;
    for (auto& note : notes) {
        ProcessScore(note);
        SlideCheck = isInSlide || SlideCheck;
        HoldCheck = isInHold || HoldCheck;
    }

    if (!wasInSlide && SlideCheck) Player->PlaySoundSlide();
    if (wasInSlide && !SlideCheck) Player->StopSoundSlide();
    if (!wasInHold && HoldCheck) Player->PlaySoundHold();
    if (wasInHold && !HoldCheck) Player->StopSoundHold();

    wasInHold = HoldCheck;
    wasInSlide = SlideCheck;
}

void AutoPlayerProcessor::MovePosition(double relative)
{
    double newTime = Player->currentSoundTime + relative;
    Status.JusticeCritical = Status.Justice = Status.Attack = Status.Miss = Status.Combo = Status.CurrentGauge = 0;

    wasInHold = isInHold = false;
    wasInSlide = isInSlide = false;
    Player->StopSoundHold();
    Player->StopSoundSlide();
    Player->RemoveSlideEffect();

    // ëóÇË: îÚÇŒÇµÇΩïîï™ÇFinishedÇ…
    // ñﬂÇµ: ì¸Ç¡ÇƒÇ≠ÇÈïîï™ÇUn-FinishedÇ…
    for (auto &note : data) {
        if (note->Type.test(SusNoteType::Hold) || note->Type.test(SusNoteType::Slide) || note->Type.test(SusNoteType::AirAction)) {
            if (note->StartTime <= newTime) note->OnTheFlyData.set(NoteAttribute::Finished);
            for (auto &extra : note->ExtraData) {
                if (extra->Type.test(SusNoteType::Tap)) continue;
                if (extra->Type.test(SusNoteType::ExTap)) continue;
                if (extra->Type.test(SusNoteType::Control)) continue;
                if (relative >= 0) {
                    if (extra->StartTime <= newTime) note->OnTheFlyData.set(NoteAttribute::Finished);
                } else {
                    if (extra->StartTime >= newTime) note->OnTheFlyData.reset(NoteAttribute::Finished);
                }
            }
        } else {
            if (relative >= 0) {
                if (note->StartTime <= newTime) note->OnTheFlyData.set(NoteAttribute::Finished);
            } else {
                if (note->StartTime >= newTime) note->OnTheFlyData.reset(NoteAttribute::Finished);
            }
        }
    }
}

void AutoPlayerProcessor::Draw()
{}

PlayStatus *AutoPlayerProcessor::GetPlayStatus()
{
    return &Status;
}

void AutoPlayerProcessor::IncrementCombo()
{
    Status.Combo++;
    Status.JusticeCritical++;
    Status.CurrentGauge += Status.GaugeDefaultMax / Status.AllNotes;
}

void AutoPlayerProcessor::ProcessScore(shared_ptr<SusDrawableNoteData> note)
{
    double relpos = (note->StartTime - Player->currentSoundTime) / Player->seenDuration;
    if (relpos >= 0 || (note->OnTheFlyData.test(NoteAttribute::Finished) && note->ExtraData.size() == 0)) return;
    auto state = note->Type.to_ulong();

    if (note->Type.test(SusNoteType::Hold)) {
        isInHold = true;
        if (!note->OnTheFlyData.test(NoteAttribute::Finished)) {
            Player->PlaySoundTap();
            Player->SpawnJudgeEffect(note, JudgeType::ShortNormal);
            IncrementCombo();
            note->OnTheFlyData.set(NoteAttribute::Finished);
        }

        for (auto &extra : note->ExtraData) {
            double pos = (extra->StartTime - Player->currentSoundTime) / Player->seenDuration;
            if (pos >= 0) continue;
            if (extra->Type.test(SusNoteType::End)) isInHold = false;
            if (extra->OnTheFlyData.test(NoteAttribute::Finished)) continue;
            if (extra->Type.test(SusNoteType::ExTap)) {
                IncrementCombo();
                extra->OnTheFlyData.set(NoteAttribute::Finished);
                return;
            }
            if (!extra->Type.test(SusNoteType::Tap)) Player->PlaySoundTap();
            Player->SpawnJudgeEffect(note, JudgeType::ShortNormal);
            IncrementCombo();
            extra->OnTheFlyData.set(NoteAttribute::Finished);
            return;
        }
    } else if (note->Type.test(SusNoteType::Slide)) {
        isInSlide = true;
        if (!note->OnTheFlyData.test(NoteAttribute::Finished)) {
            Player->PlaySoundTap();
            Player->SpawnSlideLoopEffect(note);

            IncrementCombo();
            note->OnTheFlyData.set(NoteAttribute::Finished);
            return;
        }
        for (auto &extra : note->ExtraData) {
            double pos = (extra->StartTime - Player->currentSoundTime) / Player->seenDuration;
            if (pos >= 0) continue;
            if (extra->Type.test(SusNoteType::End)) isInSlide = false;
            if (extra->Type.test(SusNoteType::Control)) continue;
            if (extra->OnTheFlyData.test(NoteAttribute::Finished)) continue;
            if (extra->Type.test(SusNoteType::ExTap)) {
                IncrementCombo();
                extra->OnTheFlyData.set(NoteAttribute::Finished);
                return;
            }
            if (!extra->Type.test(SusNoteType::Tap)) Player->PlaySoundTap();
            Player->SpawnJudgeEffect(extra, JudgeType::SlideTap);
            IncrementCombo();
            extra->OnTheFlyData.set(NoteAttribute::Finished);
            return;
        }
    } else if (note->Type.test(SusNoteType::AirAction)) {
        for (auto &extra : note->ExtraData) {
            double pos = (extra->StartTime - Player->currentSoundTime) / Player->seenDuration;
            if (pos >= 0) continue;
            if (extra->Type.test(SusNoteType::Control)) continue;
            if (extra->Type.test(SusNoteType::Tap)) continue;
            if (extra->OnTheFlyData.test(NoteAttribute::Finished)) continue;
            if (extra->Type.test(SusNoteType::ExTap)) {
                IncrementCombo();
                extra->OnTheFlyData.set(NoteAttribute::Finished);
                return;
            }
            if (pos >= 0) continue;
            Player->PlaySoundAirAction();
            Player->SpawnJudgeEffect(extra, JudgeType::Action);
            IncrementCombo();
            extra->OnTheFlyData.set(NoteAttribute::Finished);
        }
    } else if (note->Type.test(SusNoteType::Air)) {
        Player->PlaySoundAir();
        Player->SpawnJudgeEffect(note, JudgeType::ShortNormal);
        Player->SpawnJudgeEffect(note, JudgeType::ShortEx);
        IncrementCombo();
        note->OnTheFlyData.set(NoteAttribute::Finished);
    } else if (note->Type.test(SusNoteType::Tap)) {
        Player->PlaySoundTap();
        Player->SpawnJudgeEffect(note, JudgeType::ShortNormal);
        IncrementCombo();
        note->OnTheFlyData.set(NoteAttribute::Finished);
    } else if (note->Type.test(SusNoteType::ExTap)) {
        Player->PlaySoundExTap();
        Player->SpawnJudgeEffect(note, JudgeType::ShortNormal);
        Player->SpawnJudgeEffect(note, JudgeType::ShortEx);
        IncrementCombo();
        note->OnTheFlyData.set(NoteAttribute::Finished);
    } else if (note->Type.test(SusNoteType::Flick)) {
        Player->PlaySoundFlick();
        Player->SpawnJudgeEffect(note, JudgeType::ShortNormal);
        IncrementCombo();
        note->OnTheFlyData.set(NoteAttribute::Finished);
    } else {
        // åªç›Ç»Çµ 
    }
}
