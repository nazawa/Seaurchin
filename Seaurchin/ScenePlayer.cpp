#include "ScenePlayer.h"
#include "ScriptSprite.h"
#include "ExecutionManager.h"
#include "Setting.h"

using namespace std;

void RegisterPlayerScene(ExecutionManager * manager)
{
    auto engine = manager->GetScriptInterfaceUnsafe()->GetEngine();

    engine->RegisterObjectType(SU_IF_PLAY_STATUS, sizeof(PlayStatus), asOBJ_VALUE | asOBJ_POD);
    engine->RegisterObjectProperty(SU_IF_PLAY_STATUS, "uint32 JusticeCritical", asOFFSET(PlayStatus, JusticeCritical));
    engine->RegisterObjectProperty(SU_IF_PLAY_STATUS, "uint32 Justice", asOFFSET(PlayStatus, Justice));
    engine->RegisterObjectProperty(SU_IF_PLAY_STATUS, "uint32 Attack", asOFFSET(PlayStatus, Attack));
    engine->RegisterObjectProperty(SU_IF_PLAY_STATUS, "uint32 Miss", asOFFSET(PlayStatus, Miss));
    engine->RegisterObjectProperty(SU_IF_PLAY_STATUS, "uint32 AllNotes", asOFFSET(PlayStatus, AllNotes));
    engine->RegisterObjectProperty(SU_IF_PLAY_STATUS, "uint32 Combo", asOFFSET(PlayStatus, Combo));
    engine->RegisterObjectProperty(SU_IF_PLAY_STATUS, "double CurrentGauge", asOFFSET(PlayStatus, CurrentGauge));
    engine->RegisterObjectProperty(SU_IF_PLAY_STATUS, "double GaugeDefaultMax", asOFFSET(PlayStatus, GaugeDefaultMax));
    engine->RegisterObjectMethod(SU_IF_PLAY_STATUS, "void GetGaugeValue(int &out, double &out)", asMETHOD(PlayStatus, GetGaugeValue), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_PLAY_STATUS, "uint32 GetScore()", asMETHOD(PlayStatus, GetScore), asCALL_THISCALL);

    engine->RegisterObjectType(SU_IF_SCENE_PLAYER, 0, asOBJ_REF);
    engine->RegisterObjectBehaviour(SU_IF_SCENE_PLAYER, asBEHAVE_ADDREF, "void f()", asMETHOD(ScenePlayer, AddRef), asCALL_THISCALL);
    engine->RegisterObjectBehaviour(SU_IF_SCENE_PLAYER, asBEHAVE_RELEASE, "void f()", asMETHOD(ScenePlayer, Release), asCALL_THISCALL);
    engine->RegisterObjectProperty(SU_IF_SCENE_PLAYER, "int Z", asOFFSET(ScenePlayer, ZIndex));
    engine->RegisterObjectMethod(SU_IF_SPRITE, SU_IF_SCENE_PLAYER "@ opCast()", asFUNCTION((CastReferenceType<SSprite, ScenePlayer>)), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, SU_IF_SPRITE "@ opImplCast()", asFUNCTION((CastReferenceType<ScenePlayer, SSprite>)), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void Initialize()", asMETHOD(ScenePlayer, Initialize), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void AdjustCamera(double, double, double)", asMETHOD(ScenePlayer, AdjustCamera), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void SetResource(const string &in, " SU_IF_IMAGE "@)", asMETHOD(ScenePlayer, SetPlayerResource), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void SetResource(const string &in, " SU_IF_FONT "@)", asMETHOD(ScenePlayer, SetPlayerResource), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void SetResource(const string &in, " SU_IF_SOUND "@)", asMETHOD(ScenePlayer, SetPlayerResource), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void SetResource(const string &in, " SU_IF_ANIMEIMAGE "@)", asMETHOD(ScenePlayer, SetPlayerResource), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void Load()", asMETHOD(ScenePlayer, Load), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "bool IsLoadCompleted()", asMETHOD(ScenePlayer, IsLoadCompleted), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void Play()", asMETHOD(ScenePlayer, Play), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "double GetCurrentTime()", asMETHOD(ScenePlayer, GetPlayingTime), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void GetPlayStatus(" SU_IF_PLAY_STATUS " &out)", asMETHOD(ScenePlayer, GetPlayStatus), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void MovePositionBySecond(double)", asMETHOD(ScenePlayer, MovePositionBySecond), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void MovePositionByMeasure(int)", asMETHOD(ScenePlayer, MovePositionByMeasure), asCALL_THISCALL);
}


ScenePlayer::ScenePlayer(ExecutionManager *exm) : manager(exm)
{
    soundManager = manager->GetSoundManagerUnsafe();
}

ScenePlayer::~ScenePlayer()
{
    Finalize();
}

void ScenePlayer::Initialize()
{
    analyzer = make_unique<SusAnalyzer>(192);

    if (manager->ExistsData("AutoPlay") ? manager->GetData<int>("AutoPlay") : 1) {
        processor = new AutoPlayerProcessor(this);
    } else {
        processor = new PlayableProcessor(this);
    }

    auto setting = manager->GetSettingInstanceSafe();
    SeenDuration = setting->ReadValue<double>("Play", "SeenDuration", 0.8);
    SoundBufferingLatency = setting->ReadValue<double>("Sound", "BufferLatency", 0.03);

    LoadResources();
}

void ScenePlayer::Finalize()
{
    soundManager->StopGlobal(soundHoldLoop->GetSample());
    soundManager->StopGlobal(soundSlideLoop->GetSample());
    for (auto& res : resources) res.second->Release();
    soundManager->StopGlobal(bgmStream);
    delete processor;
    delete bgmStream;

    fontCombo->Release();
    DeleteGraph(hGroundBuffer);
    DeleteGraph(hBlank);
}

void ScenePlayer::LoadWorker()
{
    auto mm = manager->GetMusicsManager();
    auto scorefile = mm->GetSelectedScorePath();

    analyzer->LoadFromFile(scorefile);
    analyzer->RenderScoreData(data);
    for (auto &note : data) {
        if (note->Type.test(SusNoteType::Slide) || note->Type.test(SusNoteType::AirAction)) CalculateCurves(note);
    }
    processor->Reset();
    State = PlayingState::BgmNotLoaded;

    auto file = boost::filesystem::path(scorefile).parent_path() / ConvertUTF8ToShiftJis(analyzer->SharedMetaData.UWaveFileName);
    bgmStream = SoundStream::CreateFromFile(file.string().c_str());
    State = PlayingState::ReadyToStart;

    // 前カウントの計算
    // WaveOffsetが1小節分より長いとめんどくさそうなので差し引いてく
    BackingTime = -60.0 / analyzer->GetBpmAt(0, 0) * analyzer->GetBeatsAt(0);
    NextMetronomeTime = BackingTime;
    while (BackingTime > analyzer->SharedMetaData.WaveOffset) BackingTime -= 60.0 / analyzer->GetBpmAt(0, 0) * analyzer->GetBeatsAt(0);
    CurrentTime = BackingTime;

    isLoadCompleted = true;
}

void ScenePlayer::CalculateCurves(std::shared_ptr<SusDrawableNoteData> note)
{
    auto lastStep = note;
    //auto lastStepRelativeY = 1.0 - (lastStep->StartTime - currentTime) / seenDuration;
    double segmentsPerSecond = 20;   // Buffer上での最小の長さ
    vector<tuple<double, double>> controlPoints;    // lastStepからの時間, X中央位置(0~1)
    vector<tuple<double, double>> bezierBuffer;

    controlPoints.push_back(make_tuple(0, (lastStep->StartLane + lastStep->Length / 2.0) / 16.0));
    for (auto &slideElement : note->ExtraData) {
        if (slideElement->Type.test(SusNoteType::ExTap)) continue;
        if (slideElement->Type.test(SusNoteType::Control)) {
            auto cpi = make_tuple(slideElement->StartTime - lastStep->StartTime, (slideElement->StartLane + slideElement->Length / 2.0) / 16.0);
            controlPoints.push_back(cpi);
            continue;
        }
        // EndかStep
        controlPoints.push_back(make_tuple(slideElement->StartTime - lastStep->StartTime, (slideElement->StartLane + slideElement->Length / 2.0) / 16.0));
        int segmentPoints = segmentsPerSecond * (slideElement->StartTime - lastStep->StartTime) + 2;
        vector<tuple<double, double>> segmentPositions;
        for (int j = 0; j < segmentPoints; j++) {
            double relativeTimeInBlock = j / (double)(segmentPoints - 1);
            bezierBuffer.clear();
            copy(controlPoints.begin(), controlPoints.end(), back_inserter(bezierBuffer));
            for (int k = controlPoints.size() - 1; k >= 0; k--) {
                for (int l = 0; l < k; l++) {
                    auto derivedTime = (1.0 - relativeTimeInBlock) * get<0>(bezierBuffer[l]) + relativeTimeInBlock * get<0>(bezierBuffer[l + 1]);
                    auto derivedPosition = (1.0 - relativeTimeInBlock) * get<1>(bezierBuffer[l]) + relativeTimeInBlock * get<1>(bezierBuffer[l + 1]);
                    bezierBuffer[l] = make_tuple(derivedTime, derivedPosition);
                }
            }
            segmentPositions.push_back(bezierBuffer[0]);
        }
        curveData[slideElement] = segmentPositions;
        lastStep = slideElement;
        controlPoints.clear();
        controlPoints.push_back(make_tuple(0, (slideElement->StartLane + slideElement->Length / 2.0) / 16.0));
    }
}

void ScenePlayer::CalculateNotes(double time, double duration, double preced)
{
    judgeData.clear();
    copy_if(data.begin(), data.end(), back_inserter(judgeData), [&](shared_ptr<SusDrawableNoteData> n) {
        double ptime = time - preced;
        if (n->Type.to_ulong() & SU_NOTE_LONG_MASK) {
            // ロング
            return (ptime <= n->StartTime && n->StartTime <= time + preced)
                || (ptime <= n->StartTime + n->Duration && n->StartTime + n->Duration <= time + preced)
                || (n->StartTime <= ptime && time + preced <= n->StartTime + n->Duration);
        } else {
            // ショート
            return (ptime <= n->StartTime && n->StartTime <= time + preced);
        }
    });

    seenData.clear();
    copy_if(data.begin(), data.end(), back_inserter(seenData), [&](shared_ptr<SusDrawableNoteData> n) {
        double ptime = time - preced;
        if (n->Type.to_ulong() & SU_NOTE_LONG_MASK) {
            // ロング
            if (time > n->StartTime + n->Duration) return false;
            auto st = n->GetStateAt(time);
            // 先頭が見えてるならもちろん見える
            if (n->ModifiedPosition >= -preced && n->ModifiedPosition <= duration) return get<0>(st);
            // 先頭含めて全部-precedより手前なら見えない
            if (all_of(n->ExtraData.begin(), n->ExtraData.end(), [preced, duration](shared_ptr<SusDrawableNoteData> en) {
                if (isnan(en->ModifiedPosition)) return true;
                if (en->ModifiedPosition < -preced) return true;
                return false;
            }) && n->ModifiedPosition < -preced) return false;
            //先頭含めて全部durationより後なら見えない
            if (all_of(n->ExtraData.begin(), n->ExtraData.end(), [preced, duration](shared_ptr<SusDrawableNoteData> en) {
                if (isnan(en->ModifiedPosition)) return true;
                if (en->ModifiedPosition > duration) return true;
                return false;
            }) && n->ModifiedPosition > duration) return false;
            return true;
        } else {
            // ショート
            if (time > n->StartTime) return false;
            auto st = n->GetStateAt(time);
            if (n->ModifiedPosition < -preced || n->ModifiedPosition > duration) return false;
            return get<0>(st);
        }
    });
}

void ScenePlayer::Tick(double delta)
{
    textCombo->Tick(delta);

    for (auto& sprite : spritesPending) sprites.emplace(sprite);
    spritesPending.clear();
    auto i = sprites.begin();
    while (i != sprites.end()) {
        (*i)->Tick(delta);
        if ((*i)->IsDead) {
            (*i)->Release();
            i = sprites.erase(i);
        } else {
            i++;
        }
    }

    if (State >= PlayingState::ReadyCounting) CurrentTime += delta;
    CurrentSoundTime = CurrentTime + SoundBufferingLatency;
    if (State >= PlayingState::ReadyCounting) CalculateNotes(CurrentTime, SeenDuration, PreloadingTime);

    int pCombo = Status.Combo;
    processor->Update(judgeData);
    Status = *processor->GetPlayStatus();
    if (Status.Combo > pCombo) {
        textCombo->AbortMove(true);
        textCombo->Apply("scaleY:8.4, scaleX:8.4");
        textCombo->AddMove("scale_to(x:8, y:8, time:0.2, ease:out_quad)");
    }

    UpdateSlideEffect();
}

void ScenePlayer::ProcessSound()
{
    double ActualOffset = analyzer->SharedMetaData.WaveOffset - SoundBufferingLatency;
    if (State < PlayingState::ReadyCounting) return;

    switch (State) {
        case PlayingState::ReadyCounting:
            if (ActualOffset < 0 && CurrentTime >= ActualOffset) {
                soundManager->PlayGlobal(bgmStream);
                State = PlayingState::BgmPreceding;
            } else if (CurrentTime >= 0) {
                State = PlayingState::OnlyScoreOngoing;
            } else if (NextMetronomeTime < 0 && CurrentTime >= NextMetronomeTime) {
                //TODO: NextMetronomeにもLatency適用？
                soundManager->PlayGlobal(soundTap->GetSample());
                NextMetronomeTime += 60 / analyzer->GetBpmAt(0, 0);
            }
            break;
        case PlayingState::BgmPreceding:
            if (CurrentTime >= 0) State = PlayingState::BothOngoing;
            break;
        case PlayingState::OnlyScoreOngoing:
            if (CurrentTime >= ActualOffset) {
                soundManager->PlayGlobal(bgmStream);
                State = PlayingState::BothOngoing;
            }
            break;
        case PlayingState::BothOngoing:
            // BgmLastingは実質なさそうですね…
            //TODO: 曲の再生に応じてScoreLastingへ移行
            break;
    }
}

// スクリプト側から呼べるやつら

void ScenePlayer::Load()
{
    //thread loadThread([&] { LoadWorker(); });
    //loadThread.detach();
    LoadWorker();
}

bool ScenePlayer::IsLoadCompleted()
{
    return isLoadCompleted;
}

void ScenePlayer::SetPlayerResource(const string & name, SResource * resource)
{
    if (resources.find(name) != resources.end()) resources[name]->Release();
    resources[name] = resource;
}

void ScenePlayer::Play()
{
    if (State < PlayingState::ReadyToStart) return;
    State = PlayingState::ReadyCounting;
}

double ScenePlayer::GetPlayingTime()
{
    return CurrentTime;
}

void ScenePlayer::GetPlayStatus(PlayStatus *status)
{
    *status = Status;
}

void ScenePlayer::MovePositionBySecond(double sec)
{
    //実際に動いた時間で計算せよ
    if (State < PlayingState::BothOngoing) return;
    double gap = analyzer->SharedMetaData.WaveOffset - SoundBufferingLatency;
    double oldBgmPos = bgmStream->GetPlayingPosition();
    double oldTime = CurrentTime;
    int oldMeas = get<0>(analyzer->GetRelativeTime(CurrentTime));
    double newTime = oldTime + sec;
    double newBgmPos = oldBgmPos + (newTime - oldTime);
    newBgmPos = max(0.0, newBgmPos);
    bgmStream->SetPlayingPosition(newBgmPos);
    CurrentTime = newBgmPos + gap;
    processor->MovePosition(CurrentTime - oldTime);
}

void ScenePlayer::MovePositionByMeasure(int meas)
{
    if (State < PlayingState::BothOngoing) return;
    double gap = analyzer->SharedMetaData.WaveOffset - SoundBufferingLatency;
    double oldBgmPos = bgmStream->GetPlayingPosition();
    double oldTime = CurrentTime;
    int oldMeas = get<0>(analyzer->GetRelativeTime(CurrentTime));
    double newTime = analyzer->GetAbsoluteTime(max(0, oldMeas + meas), 0);
    double newBgmPos = oldBgmPos + (newTime - oldTime);
    newBgmPos = max(0.0, newBgmPos);
    bgmStream->SetPlayingPosition(newBgmPos);
    CurrentTime = newBgmPos + gap;
    processor->MovePosition(CurrentTime - oldTime);
}

void ScenePlayer::AdjustCamera(double cy, double cz, double ctz)
{
    cameraY += cy;
    cameraZ += cz;
    cameraTargetZ += ctz;
}