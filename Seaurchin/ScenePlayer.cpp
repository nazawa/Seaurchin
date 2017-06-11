#include "ScenePlayer.h"
#include "ScriptSprite.h"
#include "ExecutionManager.h"

using namespace std;

static uint16_t RectVertexIndices[] = { 0, 1, 3, 3, 1, 2 };
static VERTEX3D GroundVertices[] = {
    {
        VGet(SU_LANE_X_MIN, SU_LANE_Y_GROUND, SU_LANE_Z_MAX),
        VGet(0, 1, 0),
        GetColorU8(255, 255, 255, 255),
        GetColorU8(0, 0, 0, 0),
        0.0f, 0.0f,
        0.0f, 0.0f
    },
    {
        VGet(SU_LANE_X_MAX, SU_LANE_Y_GROUND, SU_LANE_Z_MAX),
        VGet(0, 1, 0),
        GetColorU8(255, 255, 255, 255),
        GetColorU8(0, 0, 0, 0),
        1.0f, 0.0f,
        0.0f, 0.0f
    },
    {
        VGet(SU_LANE_X_MAX, SU_LANE_Y_GROUND, SU_LANE_Z_MIN),
        VGet(0, 1, 0),
        GetColorU8(255, 255, 255, 255),
        GetColorU8(0, 0, 0, 0),
        1.0f, 1.0f,
        0.0f, 0.0f
    },
    {
        VGet(SU_LANE_X_MIN, SU_LANE_Y_GROUND, SU_LANE_Z_MIN),
        VGet(0, 1, 0),
        GetColorU8(255, 255, 255, 255),
        GetColorU8(0, 0, 0, 0),
        0.0f, 1.0f,
        0.0f, 0.0f
    }
};
static VERTEX3D AirVertices[] = {
    {
        VGet(SU_LANE_X_MIN, SU_LANE_Y_AIR, SU_LANE_Z_MAX),
        VGet(0, 1, 0),
        GetColorU8(255, 255, 255, 255),
        GetColorU8(0, 0, 0, 0),
        0.0f, 0.0f,
        0.0f, 0.0f
    },
    {
        VGet(SU_LANE_X_MAX, SU_LANE_Y_AIR, SU_LANE_Z_MAX),
        VGet(0, 1, 0),
        GetColorU8(255, 255, 255, 255),
        GetColorU8(0, 0, 0, 0),
        1.0f, 0.0f,
        0.0f, 0.0f
    },
    {
        VGet(SU_LANE_X_MAX, SU_LANE_Y_AIR, SU_LANE_Z_MIN),
        VGet(0, 1, 0),
        GetColorU8(255, 255, 255, 255),
        GetColorU8(0, 0, 0, 0),
        1.0f, 1.0f,
        0.0f, 0.0f
    },
    {
        VGet(SU_LANE_X_MIN, SU_LANE_Y_AIR, SU_LANE_Z_MIN),
        VGet(0, 1, 0),
        GetColorU8(255, 255, 255, 255),
        GetColorU8(0, 0, 0, 0),
        0.0f, 1.0f,
        0.0f, 0.0f
    }
};

void RegisterPlayerScene(ExecutionManager * manager)
{
    auto engine = manager->GetScriptInterfaceUnsafe()->GetEngine();

    engine->RegisterObjectType(SU_IF_SCENE_PLAYER, 0, asOBJ_REF);
    engine->RegisterObjectBehaviour(SU_IF_SCENE_PLAYER, asBEHAVE_ADDREF, "void f()", asMETHOD(ScenePlayer, AddRef), asCALL_THISCALL);
    engine->RegisterObjectBehaviour(SU_IF_SCENE_PLAYER, asBEHAVE_RELEASE, "void f()", asMETHOD(ScenePlayer, Release), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SPRITE, SU_IF_SCENE_PLAYER "@ opCast()", asFUNCTION((CastReferenceType<SSprite, ScenePlayer>)), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, SU_IF_SPRITE "@ opImplCast()", asFUNCTION((CastReferenceType<ScenePlayer, SSprite>)), asCALL_CDECL_OBJLAST);
    engine->RegisterObjectProperty(SU_IF_SCENE_PLAYER, "int Z", asOFFSET(ScenePlayer, ZIndex));
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void Initialize()", asMETHOD(ScenePlayer, Initialize), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void SetResource(const string &in, " SU_IF_IMAGE "@)", asMETHOD(ScenePlayer, SetPlayerResource), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void SetResource(const string &in, " SU_IF_FONT "@)", asMETHOD(ScenePlayer, SetPlayerResource), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void SetResource(const string &in, " SU_IF_SOUND "@)", asMETHOD(ScenePlayer, SetPlayerResource), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void Load()", asMETHOD(ScenePlayer, Load), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "bool IsLoadCompleted()", asMETHOD(ScenePlayer, IsLoadCompleted), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void Play()", asMETHOD(ScenePlayer, Play), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "double GetCurrentTime()", asMETHOD(ScenePlayer, GetPlayingTime), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "int GetSeenObjectsCount()", asMETHOD(ScenePlayer, GetSeenObjectsCount), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "int GetCurrentGauge(int &out, double &out)", asMETHOD(ScenePlayer, GetCurrentGauge), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void AdjustCamera(double, double, double)", asMETHOD(ScenePlayer, AdjustCamera), asCALL_THISCALL);
}


ScenePlayer::ScenePlayer(ExecutionManager *exm) : manager(exm)
{

}

ScenePlayer::~ScenePlayer()
{
    Finalize();
}

void ScenePlayer::Initialize()
{
    analyzer = make_unique<SusAnalyzer>(192);
    metaInfo = manager->GetMusicsManager()->Selected;

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
    soundHoldLoop->StopAll();
    soundSlideLoop->StopAll();
    for (auto& res : resources) res.second->Release();
    manager->GetSoundManagerUnsafe()->Stop(bgmStream);
    manager->GetSoundManagerUnsafe()->ReleaseSound(bgmStream);

    fontCombo->Release();
    DeleteGraph(hGroundBuffer);
    DeleteGraph(hAirBuffer);
}

void ScenePlayer::Tick(double delta)
{
    textCombo->Tick(delta);
}

void ScenePlayer::Draw()
{
    vector<tuple<double, double>> airactionStarts;
    ostringstream combo;
    currentTime = GetPlayingTime() - analyzer->SharedMetaData.WaveOffset;
    int division = 8;
    seenData.clear();
    if (isLoadCompleted) CalculateNotes(currentTime, seenDuration, precedTime);

    int pCombo = comboCount;
    combo << comboCount;
    textCombo->set_Text(combo.str());

    BEGIN_DRAW_TRANSACTION(hGroundBuffer);
    ClearDrawScreen();
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawGraph(0, 0, resources["LaneGround"]->GetHandle(), TRUE);
    for (int i = 1; i < division; i++) DrawLineAA(laneBufferX / division * i, 0, laneBufferX / division * i, laneBufferY, GetColor(255, 255, 255), 2);
    textCombo->Draw();
    FINISH_DRAW_TRANSACTION;

    // Ground
    bool SlideCheck = false;
    bool HoldCheck = false;
    BEGIN_DRAW_TRANSACTION(hGroundBuffer);
    for (auto& note : seenData) {
        auto &type = note->Type;
        if (type.test(SusNoteType::Hold)) DrawHoldNotes(note);
        if (type.test(SusNoteType::Slide)) DrawSlideNotes(note);
        if (type.test(SusNoteType::Tap)) DrawShortNotes(note);
        if (type.test(SusNoteType::ExTap)) DrawShortNotes(note);
        if (type.test(SusNoteType::Flick)) DrawShortNotes(note);
        ProcessScore(note);
        SlideCheck = isInSlide || SlideCheck;
        HoldCheck = isInHold || HoldCheck;
    }
    FINISH_DRAW_TRANSACTION;
    if (!wasInSlide && SlideCheck) soundSlideLoop->Play();
    if (wasInSlide && !SlideCheck) soundSlideLoop->StopAll();
    if (!wasInHold && HoldCheck) soundHoldLoop->Play();
    if (wasInHold && !HoldCheck) soundHoldLoop->StopAll();
    Prepare3DDrawCall();
    DrawPolygonIndexed3D(GroundVertices, 4, RectVertexIndices, 2, hGroundBuffer, TRUE);

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

    //後処理
    if (comboCount > pCombo) {
        textCombo->AbortMove(true);
        textCombo->Apply("scaleY:8.4, scaleX:8.4");
        textCombo->AddMove("scale_to(x:8, y:8, time:0.2, ease:out_quad)");
    }

    wasInHold = isInHold;
    wasInSlide = SlideCheck;
}

void ScenePlayer::LoadWorker()
{
    analyzer->LoadFromFile(metaInfo->Path.string().c_str());
    analyzer->RenderScoreData(data);
    for (auto &note : data) {
        if (note->Type.test(SusNoteType::Slide) || note->Type.test(SusNoteType::AirAction)) CalculateCurves(note);
    }
    PrecalculateNotes();
    auto file = metaInfo->WavePath;
    bgmStream = manager->GetSoundManagerUnsafe()->LoadStreamFromFile(file.string().c_str());

    isLoadCompleted = true;
}

void ScenePlayer::PrecalculateNotes()
{
    allNotesCount = 0;
    for (auto &note : data) {
        auto type = note->Type.to_ulong();
        if (type & 0b0000000011100000) {
            if (!note->Type.test(SusNoteType::AirAction)) allNotesCount++;
            for (auto &ex : note->ExtraData)
                if (
                    ex->Type.test(SusNoteType::End)
                    || ex->Type.test(SusNoteType::Step)
                    || ex->Type.test(SusNoteType::ExTap))
                    allNotesCount++;
        } else if (type & 0b0000000000011110) {
            allNotesCount++;
        }
    }
}

void ScenePlayer::IncrementCombo()
{
    comboCount++;
    currentGauge += gaugeDefaultMax / allNotesCount;
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

void ScenePlayer::DrawShortNotes(shared_ptr<SusDrawableNoteData> note)
{
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    double relpos = 1.0 - (note->StartTime - currentTime) / seenDuration;
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
    double relpos = 1.0 - (note->StartTime - currentTime) / seenDuration;
    double z = (1.0 - relpos) * SU_LANE_Z_MAX + relpos * SU_LANE_Z_MIN;
    if (relpos >= 1.0 || relpos < 0) return;

    auto length = note->Length;
    auto slane = note->StartLane;
    auto left = (1.0 - slane / 16.0) * SU_LANE_X_MIN + slane / 16.0 * SU_LANE_X_MAX;
    auto right = (1.0 - (slane + length) / 16.0) * SU_LANE_X_MIN + (slane + length) / 16.0 * SU_LANE_X_MAX;
    auto xadjust = note->Type.test(SusNoteType::Left) ? -80.0 : (note->Type.test(SusNoteType::Right) ? 80.0 : 0);
    auto role = note->Type.test(SusNoteType::Up) ? fmod(currentTime* 2.0, 0.5) : 0.5 - fmod(currentTime* 2.0, 0.5);
    auto handle = note->Type.test(SusNoteType::Up) ? imageAirUp->GetHandle() : imageAirDown->GetHandle();

    VERTEX3D vertices[] = {
        {
            VGet(left + xadjust, SU_LANE_Y_AIR, z),
            VGet(0, 0, -1),
            GetColorU8(255, 255, 255, 255),
            GetColorU8(0, 0, 0, 0),
            0.0f, role,
            0.0f, 0.0f
        },
        {
            VGet(right + xadjust, SU_LANE_Y_AIR, z),
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
    double relpos = 1.0 - (note->StartTime - currentTime) / seenDuration;
    double relendpos = 1.0;
    for (auto &ex : note->ExtraData) {
        if (ex->Type.test(SusNoteType::ExTap)) continue;
        relendpos = 1.0 - (ex->StartTime - currentTime) / seenDuration;
    }
    SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
    DrawModiGraphF(
        slane * widthPerLane, laneBufferY * relpos,
        (slane + length) * widthPerLane, laneBufferY * relpos,
        (slane + length) * widthPerLane, laneBufferY * relendpos,
        slane * widthPerLane, laneBufferY * relendpos,
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
        DrawRectRotaGraph3F(
            (slane * 2 + i) * widthPerLane / 2, laneBufferY * relendpos,
            noteImageBlockX * type, (0),
            noteImageBlockX, noteImageBlockY,
            0, noteImageBlockY / 2,
            actualNoteScaleX, actualNoteScaleY,
            0, imageHold->GetHandle(), TRUE, FALSE);
    }
}

void ScenePlayer::DrawSlideNotes(shared_ptr<SusDrawableNoteData> note)
{
    auto lastStep = note;
    auto lastStepRelativeY = 1.0 - (lastStep->StartTime - currentTime) / seenDuration;
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
        double currentStepRelativeY = 1.0 - (slideElement->StartTime - currentTime) / seenDuration;
        auto &segmentPositions = curveData[slideElement];

        auto lastSegmentPosition = segmentPositions[0];
        double lastSegmentLength = lastStep->Length;
        double lastTimeInBlock = get<0>(lastSegmentPosition) / (slideElement->StartTime - lastStep->StartTime);
        auto lastSegmentRelativeY = 1.0 - (lastStep->StartTime - currentTime) / seenDuration;
        for (auto &segmentPosition : segmentPositions) {
            if (lastSegmentPosition == segmentPosition) continue;
            double currentTimeInBlock = get<0>(segmentPosition) / (slideElement->StartTime - lastStep->StartTime);
            double currentSegmentLength = (1.0 - currentTimeInBlock) * lastStep->Length + currentTimeInBlock * slideElement->Length;
            double currentSegmentRelativeY = 1.0 - (lastStep->StartTime + get<0>(segmentPosition) - currentTime) / seenDuration;

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
    auto lastStepRelativeY = 1.0 - (lastStep->StartTime - currentTime) / seenDuration;
    double segmentLength = 128.0;   // Buffer上での最小の長さ


    for (auto &slideElement : note->ExtraData) {
        if (slideElement->Type.test(SusNoteType::Control)) continue;
        if (slideElement->Type.test(SusNoteType::ExTap)) continue;
        double currentStepRelativeY = 1.0 - (slideElement->StartTime - currentTime) / seenDuration;
        auto &segmentPositions = curveData[slideElement];

        auto lastSegmentPosition = segmentPositions[0];
        double lastSegmentLength = lastStep->Length;
        double lastTimeInBlock = get<0>(lastSegmentPosition) / (slideElement->StartTime - lastStep->StartTime);
        auto lastSegmentRelativeY = 1.0 - (lastStep->StartTime - currentTime) / seenDuration;
        for (auto &segmentPosition : segmentPositions) {
            if (lastSegmentPosition == segmentPosition) continue;
            double currentTimeInBlock = get<0>(segmentPosition) / (slideElement->StartTime - lastStep->StartTime);
            double currentSegmentLength = (1.0 - currentTimeInBlock) * lastStep->Length + currentTimeInBlock * slideElement->Length;
            double currentSegmentRelativeY = 1.0 - (lastStep->StartTime + get<0>(segmentPosition) - currentTime) / seenDuration;

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
    return make_tuple((note->StartLane + note->Length / 2.0) / 16.0, SU_LANE_Z_MAX * (note->StartTime - currentTime) / seenDuration);
}

void ScenePlayer::Prepare3DDrawCall()
{
    SetUseLighting(FALSE);
    SetCameraPositionAndTarget_UpVecY(VGet(0, cameraY, cameraZ), VGet(0, SU_LANE_Y_GROUND, cameraTargetZ));
}

void ScenePlayer::ProcessSound(double time, double duration, double preced)
{
    //役目不明
}

void ScenePlayer::ProcessScore(shared_ptr<SusDrawableNoteData> note)
{
    double relpos = (note->StartTime - currentTime) / seenDuration;
    if (relpos >= 0 || (note->OnTheFlyData.test(NoteAttribute::Finished) && note->ExtraData.size() == 0)) return;
    auto state = note->Type.to_ulong();

    if (note->Type.test(SusNoteType::Hold)) {
        isInHold = true;
        if (!note->OnTheFlyData.test(NoteAttribute::Finished)) {
            soundTap->Play();
            IncrementCombo();
            note->OnTheFlyData.set(NoteAttribute::Finished);
        }

        for (auto &extra : note->ExtraData) {
            double pos = (extra->StartTime - currentTime) / seenDuration;
            if (pos >= 0) continue;
            if (extra->Type.test(SusNoteType::End)) isInHold = false;
            if (extra->OnTheFlyData.test(NoteAttribute::Finished)) continue;
            if (extra->Type.test(SusNoteType::ExTap)) {
                IncrementCombo();
                extra->OnTheFlyData.set(NoteAttribute::Finished);
                return;
            }
            if (!extra->Type.test(SusNoteType::Tap)) soundTap->Play();
            IncrementCombo();
            extra->OnTheFlyData.set(NoteAttribute::Finished);
            return;
        }
    } else if (note->Type.test(SusNoteType::Slide)) {
        isInSlide = true;
        if (!note->OnTheFlyData.test(NoteAttribute::Finished)) {
            soundTap->Play();
            IncrementCombo();
            note->OnTheFlyData.set(NoteAttribute::Finished);
            return;
        }
        for (auto &extra : note->ExtraData) {
            double pos = (extra->StartTime - currentTime) / seenDuration;
            if (pos >= 0) continue;
            if (extra->Type.test(SusNoteType::End)) isInSlide = false;
            if (extra->Type.test(SusNoteType::Control)) continue;
            if (extra->OnTheFlyData.test(NoteAttribute::Finished)) continue;
            if (extra->Type.test(SusNoteType::ExTap)) {
                IncrementCombo();
                extra->OnTheFlyData.set(NoteAttribute::Finished);
                return;
            }
            if (!extra->Type.test(SusNoteType::Tap)) soundTap->Play();
            IncrementCombo();
            extra->OnTheFlyData.set(NoteAttribute::Finished);
            return;
        }
    } else if (note->Type.test(SusNoteType::AirAction)) {
        for (auto &extra : note->ExtraData) {
            double pos = (extra->StartTime - currentTime) / seenDuration;
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
            soundAirAction->Play();
            IncrementCombo();
            extra->OnTheFlyData.set(NoteAttribute::Finished);
        }
    } else if (note->Type.test(SusNoteType::Air)) {
        soundAir->Play();
        IncrementCombo();
        note->OnTheFlyData.set(NoteAttribute::Finished);
    } else if (note->Type.test(SusNoteType::Tap)) {
        soundTap->Play();
        IncrementCombo();
        note->OnTheFlyData.set(NoteAttribute::Finished);
    } else if (note->Type.test(SusNoteType::ExTap)) {
        soundExTap->Play();
        IncrementCombo();
        note->OnTheFlyData.set(NoteAttribute::Finished);
    } else if (note->Type.test(SusNoteType::Flick)) {
        soundFlick->Play();
        IncrementCombo();
        note->OnTheFlyData.set(NoteAttribute::Finished);
    } else {
        // 現在なし 
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
    manager->GetSoundManagerUnsafe()->Play(bgmStream);
}

double ScenePlayer::GetPlayingTime()
{
    if (!bgmStream) return 0;
    return manager->GetSoundManagerUnsafe()->GetPosition(bgmStream);
}

int ScenePlayer::GetSeenObjectsCount()
{
    return seenObjects;
}

void ScenePlayer::AdjustCamera(double cy, double cz, double ctz)
{
    cameraY += cy;
    cameraZ += cz;
    cameraTargetZ += ctz;
}

void ScenePlayer::GetCurrentGauge(int *fulfilled, double *current)
{
    *fulfilled = 0;
    *current = 0;
    double calc = currentGauge;
    double currentMax = 12000;
    while (calc >= currentMax) {
        *fulfilled += 1;
        calc -= currentMax;
        currentMax += 2000;
    }
    *current = calc / currentMax;
}
