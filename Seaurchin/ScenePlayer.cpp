#include "ScenePlayer.h"
#include "ScriptSprite.h"
#include "ExecutionManager.h"

using namespace std;

static const uint16_t RectVertexIndices[] = { 0, 1, 3, 3, 1, 2 };
static VERTEX3D GroundVertices[] = {
    { VGet(SU_LANE_X_MIN, SU_LANE_Y_GROUND, SU_LANE_Z_MAX), VGet(0, 1, 0), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.0f, 0.0f, 0.0f, 0.0f },
    { VGet(SU_LANE_X_MAX, SU_LANE_Y_GROUND, SU_LANE_Z_MAX), VGet(0, 1, 0), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 1.0f, 0.0f, 0.0f, 0.0f },
    { VGet(SU_LANE_X_MAX, SU_LANE_Y_GROUND, SU_LANE_Z_MIN), VGet(0, 1, 0), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 1.0f, 1.0f, 0.0f, 0.0f },
    { VGet(SU_LANE_X_MIN, SU_LANE_Y_GROUND, SU_LANE_Z_MIN), VGet(0, 1, 0), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.0f, 1.0f, 0.0f, 0.0f }
};

static VERTEX3D AirVertices[] = {
    { VGet(SU_LANE_X_MIN, SU_LANE_Y_AIR, SU_LANE_Z_MAX), VGet(0, 1, 0), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.0f, 0.0f, 0.0f, 0.0f },
    { VGet(SU_LANE_X_MAX, SU_LANE_Y_AIR, SU_LANE_Z_MAX), VGet(0, 1, 0), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 1.0f, 0.0f, 0.0f, 0.0f },
    { VGet(SU_LANE_X_MAX, SU_LANE_Y_AIR, SU_LANE_Z_MIN), VGet(0, 1, 0), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 1.0f, 1.0f, 0.0f, 0.0f },
    { VGet(SU_LANE_X_MIN, SU_LANE_Y_AIR, SU_LANE_Z_MIN), VGet(0, 1, 0), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.0f, 1.0f, 0.0f, 0.0f }
};

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
    State = -2;
}

ScenePlayer::~ScenePlayer()
{
    Finalize();
}

void ScenePlayer::Initialize()
{
    analyzer = make_unique<SusAnalyzer>(192);
    processor = new PlayableProcessor(this);

    // 2^x制限があるのでここで計算
    double bufferY = 2;
    while (laneBufferY > bufferY) bufferY *= 2;
    float bufferV = laneBufferY / bufferY;
    for (int i = 2; i < 4; i++) GroundVertices[i].v = bufferV;
    for (int i = 2; i < 4; i++) AirVertices[i].v = bufferV;
    hGroundBuffer = MakeScreen(laneBufferX, bufferY, TRUE);
    hAirBuffer = MakeScreen(laneBufferX, bufferY, TRUE);

    imageTap = dynamic_cast<SImage*>(resources["Tap"]);
    imageExTap = dynamic_cast<SImage*>(resources["ExTap"]);
    imageFlick = dynamic_cast<SImage*>(resources["Flick"]);
    imageAirUp = dynamic_cast<SImage*>(resources["AirUp"]);
    imageAirDown = dynamic_cast<SImage*>(resources["AirDown"]);
    imageHold = dynamic_cast<SImage*>(resources["Hold"]);
    imageHoldStrut = dynamic_cast<SImage*>(resources["HoldStrut"]);
    imageSlide = dynamic_cast<SImage*>(resources["Slide"]);
    imageSlideStrut = dynamic_cast<SImage*>(resources["SlideStrut"]);
    imageAirAction = dynamic_cast<SImage*>(resources["AirAction"]);
    animeTap = dynamic_cast<SAnimatedImage*>(resources["EffectTap"]);
    animeExTap = dynamic_cast<SAnimatedImage*>(resources["EffectExTap"]);
    animeSlideTap = dynamic_cast<SAnimatedImage*>(resources["EffectSlideTap"]);
    animeSlideLoop = dynamic_cast<SAnimatedImage*>(resources["EffectSlideLoop"]);
    animeAirAction = dynamic_cast<SAnimatedImage*>(resources["EffectAirAction"]);

    soundTap = dynamic_cast<SSound*>(resources["SoundTap"]);
    soundExTap = dynamic_cast<SSound*>(resources["SoundExTap"]);
    soundFlick = dynamic_cast<SSound*>(resources["SoundFlick"]);
    soundAir = dynamic_cast<SSound*>(resources["SoundAir"]);
    soundAirAction = dynamic_cast<SSound*>(resources["SoundAirAction"]);
    soundSlideLoop = dynamic_cast<SSound*>(resources["SoundSlideLoop"]);
    soundHoldLoop = dynamic_cast<SSound*>(resources["SoundHoldLoop"]);
    fontCombo = dynamic_cast<SFont*>(resources["FontCombo"]);

    fontCombo->AddRef();
    textCombo = STextSprite::Factory(fontCombo, "0000");
    textCombo->SetAlignment(STextAlign::Center, STextAlign::Center);
    textCombo->Apply("x:512, y:3200, scaleX:8, scaleY:8");
    soundHoldLoop->SetLoop(true);
    soundSlideLoop->SetLoop(true);
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
    DeleteGraph(hAirBuffer);
}


void ScenePlayer::AddSprite(SSprite *sprite)
{
    spritesPending.push_back(sprite);
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
    while(BackingTime > analyzer->SharedMetaData.WaveOffset) BackingTime -= 60.0 / analyzer->GetBpmAt(0, 0) * analyzer->GetBeatsAt(0);
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
    copy_if(data.begin(), data.end(), back_inserter(seenData), [&](shared_ptr<SusDrawableNoteData> n) {
        double ptime = time - preced;
        if (n->Type.to_ulong() & 0b0000000011100000) {
            // ロング
            return (ptime <= n->StartTime && n->StartTime <= time + duration)
                || (ptime <= n->StartTime + n->Duration && n->StartTime + n->Duration <= time + duration)
                || (n->StartTime <= ptime && time + duration <= n->StartTime + n->Duration);
        } else {
            // ショート
            return (ptime <= n->StartTime && n->StartTime <= time + duration);
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

    if (State != -2) BackingTime += delta;
    CurrentTime = GetPlayingTime() - analyzer->SharedMetaData.WaveOffset;
    CurrentSoundTime = CurrentTime + SoundBufferingLatency;
    seenData.clear();
    if (State >= -1) CalculateNotes(CurrentTime, SeenDuration, PreloadingTime);

    int pCombo = Status.Combo;
    processor->Update(seenData);
    Status = *processor->GetPlayStatus();
    if (Status.Combo > pCombo) {
        textCombo->AbortMove(true);
        textCombo->Apply("scaleY:8.4, scaleX:8.4");
        textCombo->AddMove("scale_to(x:8, y:8, time:0.2, ease:out_quad)");
    }

    UpdateSlideEffect();
}

void ScenePlayer::Draw()
{
    vector<tuple<double, double>> airactionStarts;
    ostringstream combo;
    int division = 8;
    combo << Status.Combo;
    textCombo->set_Text(combo.str());

    ProcessSound();

    BEGIN_DRAW_TRANSACTION(hGroundBuffer);
    ClearDrawScreen();
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawGraph(0, 0, resources["LaneGround"]->GetHandle(), TRUE);
    processor->Draw();
    textCombo->Draw();

    DrawMeasureLines();
    for (auto& note : seenData) {
        auto &type = note->Type;
        if (type.test(SusNoteType::Hold)) DrawHoldNotes(note);
        if (type.test(SusNoteType::Slide)) DrawSlideNotes(note);
        if (type.test(SusNoteType::Tap)) DrawShortNotes(note);
        if (type.test(SusNoteType::ExTap)) DrawShortNotes(note);
        if (type.test(SusNoteType::Flick)) DrawShortNotes(note);
    }
    FINISH_DRAW_TRANSACTION;
    Prepare3DDrawCall();
    DrawPolygonIndexed3D(GroundVertices, 4, RectVertexIndices, 2, hGroundBuffer, TRUE);
    for (auto& i : sprites) i->Draw();
    //Air

    BEGIN_DRAW_TRANSACTION(hAirBuffer);
    ClearDrawScreen();
    for (auto& note : seenData) if (note->Type.test(SusNoteType::AirAction)) airactionStarts.push_back(DrawAirActionNotes(note));
    FINISH_DRAW_TRANSACTION;
    Prepare3DDrawCall();
    DrawPolygonIndexed3D(AirVertices, 4, RectVertexIndices, 2, hAirBuffer, TRUE);
    for (auto& position : airactionStarts) {
        double x = (1.0 - get<0>(position)) * SU_LANE_X_MIN + get<0>(position) * SU_LANE_X_MAX;
        DrawLine3D(VGet(x, SU_LANE_Y_AIR, get<1>(position)), VGet(x, SU_LANE_Y_GROUND, get<1>(position)), airActionLineColor);
    }
    for (auto& note : seenData) if (note->Type.test(SusNoteType::Air)) DrawAirNotes(note);
}

// position は 0 ~ 16
void ScenePlayer::SpawnJudgeEffect(shared_ptr<SusDrawableNoteData> target, JudgeType type)
{
    auto position = target->StartLane + target->Length / 2.0;
    auto x = (1.0 - position / 16.0) * SU_LANE_X_MIN + (position / 16.0) *  SU_LANE_X_MAX;
    switch (type) {
        case JudgeType::ShortNormal: {
            auto spawnAt = ConvWorldPosToScreenPos(VGet(x, SU_LANE_Y_GROUND, SU_LANE_Z_MIN));
            animeTap->AddRef();
            auto sp = SAnimeSprite::Factory(animeTap);
            sp->Apply("origX:128, origY:224");
            sp->Transform.X = spawnAt.x;
            sp->Transform.Y = spawnAt.y;
            AddSprite(sp);
            break;
        }
        case JudgeType::ShortEx: {
            auto spawnAt = ConvWorldPosToScreenPos(VGet(x, SU_LANE_Y_GROUND, SU_LANE_Z_MIN));
            animeExTap->AddRef();
            auto sp = SAnimeSprite::Factory(animeExTap);
            sp->Apply("origX:128, origY:256");
            sp->Transform.X = spawnAt.x;
            sp->Transform.Y = spawnAt.y;
            sp->Transform.ScaleX = target->Length / 6.0;
            AddSprite(sp);
            break;
        }
        case JudgeType::SlideTap: {
            auto spawnAt = ConvWorldPosToScreenPos(VGet(x, SU_LANE_Y_GROUND, SU_LANE_Z_MIN));
            animeSlideTap->AddRef();
            auto sp = SAnimeSprite::Factory(animeSlideTap);
            sp->Apply("origX:128, origY:224");
            sp->Transform.X = spawnAt.x;
            sp->Transform.Y = spawnAt.y;
            AddSprite(sp);
            break;
        }
        case JudgeType::Action: {
            auto spawnAt = ConvWorldPosToScreenPos(VGet(x, SU_LANE_Y_AIR, SU_LANE_Z_MIN));
            animeAirAction->AddRef();
            auto sp = SAnimeSprite::Factory(animeAirAction);
            sp->Apply("origX:128, origY:128");
            sp->Transform.X = spawnAt.x;
            sp->Transform.Y = spawnAt.y;
            AddSprite(sp);
            break;
        }
    }
}

void ScenePlayer::SpawnSlideLoopEffect(shared_ptr<SusDrawableNoteData> target)
{
    animeSlideLoop->AddRef();
    imageTap->AddRef();
    auto loopefx = SAnimeSprite::Factory(animeSlideLoop);
    loopefx->Apply("origX:128, origY:224");
    SpawnJudgeEffect(target, JudgeType::SlideTap);
    loopefx->SetLoopCount(-1);
    SlideEffects[target] = loopefx;
    AddSprite(loopefx);
}

void ScenePlayer::RemoveSlideEffect()
{
    auto it = SlideEffects.begin();
    while (it != SlideEffects.end()) {
        auto note = (*it).first;
        auto effect = (*it).second;
        effect->Dismiss();
        it = SlideEffects.erase(it);
    }
}

void ScenePlayer::UpdateSlideEffect()
{
    auto it = SlideEffects.begin();
    while (it != SlideEffects.end()) {
        auto note = (*it).first;
        auto effect = (*it).second;
        if (CurrentTime >= note->StartTime + note->Duration) {
            effect->Dismiss();
            it = SlideEffects.erase(it);
            continue;
        }
        auto last = note;

        for (auto &slideElement : note->ExtraData) {
            if (slideElement->Type.test(SusNoteType::Control)) continue;
            if (slideElement->Type.test(SusNoteType::ExTap)) continue;
            if (CurrentTime >= slideElement->StartTime) {
                last = slideElement;
                continue;
            }
            auto &segmentPositions = curveData[slideElement];

            auto lastSegmentPosition = segmentPositions[0];
            double lastTimeInBlock = get<0>(lastSegmentPosition) / (slideElement->StartTime - last->StartTime);
            bool comp = false;
            for (auto &segmentPosition : segmentPositions) {
                if (lastSegmentPosition == segmentPosition) continue;
                double currentTimeInBlock = get<0>(segmentPosition) / (slideElement->StartTime - last->StartTime);
                if (CurrentTime >= ((1.0 - currentTimeInBlock) * last->StartTime + currentTimeInBlock * slideElement->StartTime)) {
                    lastSegmentPosition = segmentPosition;
                    lastTimeInBlock = currentTimeInBlock;
                    continue;
                }
                double lst = (1.0 - lastTimeInBlock) * last->StartTime + lastTimeInBlock * slideElement->StartTime;
                double cst = (1.0 - currentTimeInBlock) * last->StartTime + currentTimeInBlock * slideElement->StartTime;
                double t = (CurrentTime - lst) / (cst - lst);
                double x = (1.0 - t) * get<1>(lastSegmentPosition) + t * get<1>(segmentPosition);
                double absx = (1.0 - x) * SU_LANE_X_MIN + x * SU_LANE_X_MAX;
                auto at = ConvWorldPosToScreenPos(VGet(absx, SU_LANE_Y_GROUND, SU_LANE_Z_MIN));
                effect->Transform.X = at.x;
                effect->Transform.Y = at.y;
                comp = true;
                break;
            }
            if (comp) break;
        }
        it++;
    }
}


void ScenePlayer::DrawShortNotes(shared_ptr<SusDrawableNoteData> note)
{
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    double relpos = 1.0 - (note->StartTime - CurrentTime) / SeenDuration;
    auto length = note->Length;
    auto slane = note->StartLane;
    int handleToDraw = 0;

    if (note->Type.test(SusNoteType::Tap)) {
        handleToDraw = imageTap->GetHandle();
    } else if (note->Type.test(SusNoteType::ExTap)) {
        handleToDraw = imageExTap->GetHandle();
    } else if (note->Type.test(SusNoteType::Flick)) {
        handleToDraw = imageFlick->GetHandle();
    }

    //64*3 x 64 を描画するから1/2でやる必要がある
    for (int i = 0; i < length * 2; i++) {
        int type = i ? (i == length * 2 - 1 ? 2 : 1) : 0;
        DrawRectRotaGraph3F((slane * 2 + i) * widthPerLane / 2, laneBufferY * relpos, noteImageBlockX * type, (0), noteImageBlockX, noteImageBlockY, 0, noteImageBlockY / 2, actualNoteScaleX, actualNoteScaleY, 0, handleToDraw, TRUE, FALSE);
    }
}

void ScenePlayer::DrawAirNotes(shared_ptr<SusDrawableNoteData> note)
{
    double relpos = 1.0 - (note->StartTime - CurrentTime) / SeenDuration;
    double z = (1.0 - relpos) * SU_LANE_Z_MAX + relpos * SU_LANE_Z_MIN;
    if (relpos >= 1.0 || relpos < 0) return;

    auto length = note->Length;
    auto slane = note->StartLane;
    auto left = (1.0 - slane / 16.0) * SU_LANE_X_MIN + slane / 16.0 * SU_LANE_X_MAX;
    auto right = (1.0 - (slane + length) / 16.0) * SU_LANE_X_MIN + (slane + length) / 16.0 * SU_LANE_X_MAX;
    auto xadjust = note->Type.test(SusNoteType::Left) ? -80.0 : (note->Type.test(SusNoteType::Right) ? 80.0 : 0);
    auto role = note->Type.test(SusNoteType::Up) ? fmod(CurrentTime* 2.0, 0.5) : 0.5 - fmod(CurrentTime* 2.0, 0.5);
    auto handle = note->Type.test(SusNoteType::Up) ? imageAirUp->GetHandle() : imageAirDown->GetHandle();

    VERTEX3D vertices[] = {
        {
            VGet(left + xadjust, SU_LANE_Y_AIRINDICATE, z),
            VGet(0, 0, -1),
            GetColorU8(255, 255, 255, 255),
            GetColorU8(0, 0, 0, 0),
            0.0f, role,
            0.0f, 0.0f
        },
        {
            VGet(right + xadjust, SU_LANE_Y_AIRINDICATE, z),
            VGet(0, 0, -1),
            GetColorU8(255, 255, 255, 255),
            GetColorU8(0, 0, 0, 0),
            1.0f, role,
            0.0f, 0.0f
        },
        {
            VGet(right, SU_LANE_Y_GROUND, z),
            VGet(0, 0, -1),
            GetColorU8(255, 255, 255, 255),
            GetColorU8(0, 0, 0, 0),
            1.0f, role + 0.5f,
            0.0f, 0.0f
        },
        {
            VGet(left, SU_LANE_Y_GROUND, z),
            VGet(0, 0, -1),
            GetColorU8(255, 255, 255, 255),
            GetColorU8(0, 0, 0, 0),
            0.0f, role + 0.5f,
            0.0f, 0.0f
        }
    };
    Prepare3DDrawCall();
    DrawPolygonIndexed3D(vertices, 4, RectVertexIndices, 2, handle, TRUE);
}

void ScenePlayer::DrawHoldNotes(shared_ptr<SusDrawableNoteData> note)
{
    auto length = note->Length;
    auto slane = note->StartLane;
    double relpos = 1.0 - (note->StartTime - CurrentTime) / SeenDuration;
    //中身だけ先に描画
    auto endpoint = note->ExtraData.back();
    double reltailpos = 1.0 - (endpoint->StartTime - CurrentTime) / SeenDuration;
    SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
    DrawModiGraphF(
        slane * widthPerLane, laneBufferY * relpos,
        (slane + length) * widthPerLane, laneBufferY * relpos,
        (slane + length) * widthPerLane, laneBufferY * reltailpos,
        slane * widthPerLane, laneBufferY * reltailpos,
        imageHoldStrut->GetHandle(), TRUE
    );

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    for (int i = 0; i < length * 2; i++) {
        int type = i ? (i == length * 2 - 1 ? 2 : 1) : 0;
        DrawRectRotaGraph3F(
            (slane * 2 + i) * widthPerLane / 2, laneBufferY * relpos,
            noteImageBlockX * type, (0),
            noteImageBlockX, noteImageBlockY,
            0, noteImageBlockY / 2,
            actualNoteScaleX, actualNoteScaleY,
            0, imageHold->GetHandle(), TRUE, FALSE);
    }

    for (auto &ex : note->ExtraData) {
        if (ex->Type.test(SusNoteType::ExTap)) continue;
        double relendpos = 1.0 - (ex->StartTime - CurrentTime) / SeenDuration;
        for (int i = 0; i < length * 2; i++) {
            int type = i ? (i == length * 2 - 1 ? 2 : 1) : 0;
            DrawRectRotaGraph3F(
                (slane * 2 + i) * widthPerLane / 2, laneBufferY * relendpos,
                noteImageBlockX * type, (0),
                noteImageBlockX, noteImageBlockY,
                0, noteImageBlockY / 2,
                actualNoteScaleX, actualNoteScaleY,
                0, imageHold->GetHandle(), TRUE, FALSE);
        }
    }
}

void ScenePlayer::DrawSlideNotes(shared_ptr<SusDrawableNoteData> note)
{
    auto lastStep = note;
    auto lastStepRelativeY = 1.0 - (lastStep->StartTime - CurrentTime) / SeenDuration;
    double segmentLength = 128.0;   // Buffer上での最小の長さ

    for (int i = 0; i < lastStep->Length * 2; i++) {
        int type = i ? (i == lastStep->Length * 2 - 1 ? 2 : 1) : 0;
        DrawRectRotaGraph3F(
            (lastStep->StartLane * 2 + i) * widthPerLane / 2, laneBufferY * lastStepRelativeY,
            noteImageBlockX * type, (0),
            noteImageBlockX, noteImageBlockY,
            0, noteImageBlockY / 2,
            actualNoteScaleX, actualNoteScaleY,
            0, imageSlide->GetHandle(), TRUE, FALSE);
    }

    for (auto &slideElement : note->ExtraData) {
        if (slideElement->Type.test(SusNoteType::Control)) continue;
        if (slideElement->Type.test(SusNoteType::ExTap)) continue;
        double currentStepRelativeY = 1.0 - (slideElement->StartTime - CurrentTime) / SeenDuration;
        auto &segmentPositions = curveData[slideElement];

        auto lastSegmentPosition = segmentPositions[0];
        double lastSegmentLength = lastStep->Length;
        double lastTimeInBlock = get<0>(lastSegmentPosition) / (slideElement->StartTime - lastStep->StartTime);
        auto lastSegmentRelativeY = 1.0 - (lastStep->StartTime - CurrentTime) / SeenDuration;
        for (auto &segmentPosition : segmentPositions) {
            if (lastSegmentPosition == segmentPosition) continue;
            double currentTimeInBlock = get<0>(segmentPosition) / (slideElement->StartTime - lastStep->StartTime);
            double currentSegmentLength = (1.0 - currentTimeInBlock) * lastStep->Length + currentTimeInBlock * slideElement->Length;
            double currentSegmentRelativeY = 1.0 - (lastStep->StartTime + get<0>(segmentPosition) - CurrentTime) / SeenDuration;

            SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
            DrawRectModiGraphF(
                get<1>(lastSegmentPosition) * laneBufferX - lastSegmentLength / 2 * widthPerLane, laneBufferY * lastSegmentRelativeY,
                get<1>(lastSegmentPosition) * laneBufferX + lastSegmentLength / 2 * widthPerLane, laneBufferY * lastSegmentRelativeY,
                get<1>(segmentPosition) * laneBufferX + currentSegmentLength / 2 * widthPerLane, laneBufferY * currentSegmentRelativeY,
                get<1>(segmentPosition) * laneBufferX - currentSegmentLength / 2 * widthPerLane, laneBufferY * currentSegmentRelativeY,
                0, 192.0 * lastTimeInBlock, noteImageBlockX, 192.0 * (currentTimeInBlock - lastTimeInBlock),
                imageSlideStrut->GetHandle(), TRUE
            );
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
            DrawLineAA(
                get<1>(lastSegmentPosition) * laneBufferX, laneBufferY * lastSegmentRelativeY,
                get<1>(segmentPosition) * laneBufferX, laneBufferY * currentSegmentRelativeY,
                slideLineColor, 16);

            lastSegmentPosition = segmentPosition;
            lastSegmentLength = currentSegmentLength;
            lastSegmentRelativeY = currentSegmentRelativeY;
            lastTimeInBlock = currentTimeInBlock;
        }
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
        for (int i = 0; i < slideElement->Length * 2; i++) {
            if (slideElement->Type.test(SusNoteType::Tap)) continue;    //Invisible Step
            int type = i ? (i == slideElement->Length * 2 - 1 ? 2 : 1) : 0;
            DrawRectRotaGraph3F(
                (slideElement->StartLane * 2 + i) * widthPerLane / 2, laneBufferY * currentStepRelativeY,
                noteImageBlockX * type, (0),
                noteImageBlockX, noteImageBlockY,
                0, noteImageBlockY / 2,
                actualNoteScaleX, actualNoteScaleY,
                0, imageSlide->GetHandle(), TRUE, FALSE);
        }
        lastStep = slideElement;
        lastStepRelativeY = currentStepRelativeY;
    }
}

tuple<double, double> ScenePlayer::DrawAirActionNotes(shared_ptr<SusDrawableNoteData> note)
{
    auto lastStep = note;
    auto lastStepRelativeY = 1.0 - (lastStep->StartTime - CurrentTime) / SeenDuration;
    double segmentLength = 128.0;   // Buffer上での最小の長さ


    for (auto &slideElement : note->ExtraData) {
        if (slideElement->Type.test(SusNoteType::Control)) continue;
        if (slideElement->Type.test(SusNoteType::ExTap)) continue;
        double currentStepRelativeY = 1.0 - (slideElement->StartTime - CurrentTime) / SeenDuration;
        auto &segmentPositions = curveData[slideElement];

        auto lastSegmentPosition = segmentPositions[0];
        double lastSegmentLength = lastStep->Length;
        double lastTimeInBlock = get<0>(lastSegmentPosition) / (slideElement->StartTime - lastStep->StartTime);
        auto lastSegmentRelativeY = 1.0 - (lastStep->StartTime - CurrentTime) / SeenDuration;
        for (auto &segmentPosition : segmentPositions) {
            if (lastSegmentPosition == segmentPosition) continue;
            double currentTimeInBlock = get<0>(segmentPosition) / (slideElement->StartTime - lastStep->StartTime);
            double currentSegmentLength = (1.0 - currentTimeInBlock) * lastStep->Length + currentTimeInBlock * slideElement->Length;
            double currentSegmentRelativeY = 1.0 - (lastStep->StartTime + get<0>(segmentPosition) - CurrentTime) / SeenDuration;

            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
            DrawLineAA(
                get<1>(lastSegmentPosition) * laneBufferX, laneBufferY * lastSegmentRelativeY,
                get<1>(segmentPosition) * laneBufferX, laneBufferY * currentSegmentRelativeY,
                airActionLineColor, 16);

            lastSegmentPosition = segmentPosition;
            lastSegmentLength = currentSegmentLength;
            lastSegmentRelativeY = currentSegmentRelativeY;
            lastTimeInBlock = currentTimeInBlock;
        }
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
        for (int i = 0; i < slideElement->Length * 2; i++) {
            if (slideElement->Type.test(SusNoteType::Tap)) continue;    //Invisible Step
            int type = i ? (i == slideElement->Length * 2 - 1 ? 2 : 1) : 0;
            DrawRectRotaGraph3F(
                (slideElement->StartLane * 2 + i) * widthPerLane / 2, laneBufferY * currentStepRelativeY,
                noteImageBlockX * type, (0),
                noteImageBlockX, noteImageBlockY,
                0, noteImageBlockY / 2,
                actualNoteScaleX, actualNoteScaleY,
                0, imageAirAction->GetHandle(), TRUE, FALSE);
        }
        lastStep = slideElement;
        lastStepRelativeY = currentStepRelativeY;
    }
    return make_tuple((note->StartLane + note->Length / 2.0) / 16.0, SU_LANE_Z_MAX * (note->StartTime - CurrentTime) / SeenDuration);
}

void ScenePlayer::DrawMeasureLines()
{
    int division = 8;
    for (int i = 1; i < division; i++) DrawLineAA(laneBufferX / division * i, 0, laneBufferX / division * i, laneBufferY, GetColor(255, 255, 255), 3);

    auto rbeg = analyzer->GetRelativeTime(CurrentTime);
    auto rend = analyzer->GetRelativeTime(CurrentTime + SeenDuration);
    for (int i = get<0>(rbeg); i < get<0>(rend) + 1; i++) {
        auto pos = analyzer->GetAbsoluteTime(i, 0);
        double relpos = 1.0 - (pos - CurrentTime) / SeenDuration;
        DrawLineAA(0, relpos * laneBufferY, laneBufferX, relpos * laneBufferY, GetColor(255, 255, 255), 6);
    }
}

void ScenePlayer::Prepare3DDrawCall()
{
    SetUseLighting(FALSE);
    SetCameraPositionAndTarget_UpVecY(VGet(0, cameraY, cameraZ), VGet(0, SU_LANE_Y_GROUND, cameraTargetZ));
}

void ScenePlayer::ProcessSound()
{
    if (State == -1 && BackingTime >= 0) {
        soundManager->PlayGlobal(bgmStream);
        State = 0;
    }
    if (State != 0 && NextMetronomeTime <= BackingTime && NextMetronomeTime < 0) {
        soundManager->PlayGlobal(soundTap->GetSample());
        NextMetronomeTime += 60 / analyzer->GetBpmAt(0, 0);
    }
    if (State == 0) {
        BackingTime = bgmStream->GetPlayingPosition();
    }
}

// スクリプト側から呼べるやつら

void ScenePlayer::Load()
{
    thread loadThread([&] { LoadWorker(); });
    loadThread.detach();
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
    State = -1;
}

double ScenePlayer::GetPlayingTime()
{
    // バッファリングの影響なのかBGM再生位置はTickから計算されるより33~36ms遅れる
    if (State != 0) {
        return BackingTime - SoundBufferingLatency;
    } else {
        return bgmStream->GetPlayingPosition();
    }
}

void ScenePlayer::GetPlayStatus(PlayStatus *status)
{
    *status = Status;
}

void ScenePlayer::MovePositionBySecond(double sec)
{
    if (State != 0) return;
    double newTime = CurrentTime + sec;
    auto pos = BASS_ChannelSeconds2Bytes(bgmStream->GetSoundHandle(), newTime);
    BASS_ChannelSetPosition(bgmStream->GetSoundHandle(), pos, BASS_POS_BYTE);
    processor->MovePosition(sec);
}

void ScenePlayer::MovePositionByMeasure(int meas)
{
    if (State != 0) return;
    auto cp = analyzer->GetRelativeTime(CurrentTime);
    double newTime = analyzer->GetAbsoluteTime(get<0>(cp) + meas, 0);
    double rel = newTime - CurrentTime;
    auto pos = BASS_ChannelSeconds2Bytes(bgmStream->GetSoundHandle(), newTime);
    BASS_ChannelSetPosition(bgmStream->GetSoundHandle(), pos, BASS_POS_BYTE);
    processor->MovePosition(rel);
}

void ScenePlayer::AdjustCamera(double cy, double cz, double ctz)
{
    cameraY += cy;
    cameraZ += cz;
    cameraTargetZ += ctz;
}