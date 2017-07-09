#include "ScenePlayer.h"
#include "ScriptSprite.h"
#include "ExecutionManager.h"
#include "Setting.h"

using namespace std;

static const uint16_t RectVertexIndices[] = { 0, 1, 3, 3, 1, 2 };
static VERTEX3D GroundVertices[] = {
    { VGet(SU_LANE_X_MIN, SU_LANE_Y_GROUND, SU_LANE_Z_MAX), VGet(0, 1, 0), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.0f, 0.0f, 0.0f, 0.0f },
    { VGet(SU_LANE_X_MAX, SU_LANE_Y_GROUND, SU_LANE_Z_MAX), VGet(0, 1, 0), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 1.0f, 0.0f, 0.0f, 0.0f },
    { VGet(SU_LANE_X_MAX, SU_LANE_Y_GROUND, SU_LANE_Z_MIN_EXT), VGet(0, 1, 0), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 1.0f, 1.0f, 0.0f, 0.0f },
    { VGet(SU_LANE_X_MIN, SU_LANE_Y_GROUND, SU_LANE_Z_MIN_EXT), VGet(0, 1, 0), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.0f, 1.0f, 0.0f, 0.0f }
};


void ScenePlayer::LoadResources()
{
    // 2^x制限があるのでここで計算
    int exty = laneBufferX * SU_LANE_ASPECT_EXT;
    double bufferY = 2;
    while (exty > bufferY) bufferY *= 2;
    float bufferV = exty / bufferY;
    for (int i = 2; i < 4; i++) GroundVertices[i].v = bufferV;
    hGroundBuffer = MakeScreen(laneBufferX, bufferY, TRUE);
    hBlank = MakeScreen(128, 128, FALSE);
    BEGIN_DRAW_TRANSACTION(hBlank);
    DrawBox(0, 0, 128, 128, GetColor(255, 255, 255), TRUE);
    FINISH_DRAW_TRANSACTION;

    imageLaneGround = dynamic_cast<SImage*>(resources["LaneGround"]);
    imageLaneJudgeLine = dynamic_cast<SImage*>(resources["LaneJudgeLine"]);
    imageTap = dynamic_cast<SImage*>(resources["Tap"]);
    imageExTap = dynamic_cast<SImage*>(resources["ExTap"]);
    imageFlick = dynamic_cast<SImage*>(resources["Flick"]);
    imageHellTap = dynamic_cast<SImage*>(resources["HellTap"]);
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

void ScenePlayer::AddSprite(SSprite *sprite)
{
    spritesPending.push_back(sprite);
}

void ScenePlayer::Draw()
{
    ostringstream combo;
    int division = 8;
    combo << Status.Combo;
    textCombo->set_Text(combo.str());

    ProcessSound();

    BEGIN_DRAW_TRANSACTION(hGroundBuffer);
    ClearDrawScreen();
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawGraph(0, 0, imageLaneGround->GetHandle(), TRUE);
    SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
    DrawRectRotaGraph3F(
        0, laneBufferY,
        0, 0,
        imageLaneJudgeLine->get_Width(), imageLaneJudgeLine->get_Height(),
        0, imageLaneJudgeLine->get_Height() / 2,
        1, 1, 0,
        imageLaneJudgeLine->GetHandle(), TRUE, FALSE);
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
        if (type.test(SusNoteType::HellTap)) DrawShortNotes(note);
    }
    FINISH_DRAW_TRANSACTION;
    Prepare3DDrawCall();
    DrawPolygonIndexed3D(GroundVertices, 4, RectVertexIndices, 2, hGroundBuffer, TRUE);
    for (auto& i : sprites) i->Draw();

    //3D系ノーツ
    Prepare3DDrawCall();
    for (auto& note : seenData) {
        if (note->Type.test(SusNoteType::AirAction)) DrawAirActionNotes(note);
        if (note->Type.test(SusNoteType::Air)) DrawAirNotes(note);
    }

    if (AirActionShown) {
        SetDrawBlendMode(DX_BLENDMODE_ADD, 192);
        SetUseZBuffer3D(FALSE);
        DrawTriangle3D(
            VGet(SU_LANE_X_MIN_EXT, SU_LANE_Y_AIR - 5, SU_LANE_Z_MIN - 5),
            VGet(SU_LANE_X_MIN_EXT, SU_LANE_Y_AIR + 5, SU_LANE_Z_MIN + 5),
            VGet(SU_LANE_X_MAX_EXT, SU_LANE_Y_AIR + 5, SU_LANE_Z_MIN + 5),
            airActionJudgeColor, TRUE);
        DrawTriangle3D(
            VGet(SU_LANE_X_MIN_EXT, SU_LANE_Y_AIR - 5, SU_LANE_Z_MIN - 5),
            VGet(SU_LANE_X_MAX_EXT, SU_LANE_Y_AIR + 5, SU_LANE_Z_MIN + 5),
            VGet(SU_LANE_X_MAX_EXT, SU_LANE_Y_AIR - 5, SU_LANE_Z_MIN - 5),
            airActionJudgeColor, TRUE);
    }

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
    double relpos = 1.0 - note->ModifiedPosition / SeenDuration;
    auto length = note->Length;
    auto slane = note->StartLane;
    int handleToDraw = 0;

    if (note->Type.test(SusNoteType::Tap)) {
        handleToDraw = imageTap->GetHandle();
    } else if (note->Type.test(SusNoteType::ExTap)) {
        handleToDraw = imageExTap->GetHandle();
    } else if (note->Type.test(SusNoteType::Flick)) {
        handleToDraw = imageFlick->GetHandle();
    } else if (note->Type.test(SusNoteType::HellTap)) {
        handleToDraw = imageHellTap->GetHandle();
    }

    //64*3 x 64 を描画するから1/2でやる必要がある
    DrawTap(slane, length, relpos, handleToDraw);
}

void ScenePlayer::DrawAirNotes(shared_ptr<SusDrawableNoteData> note)
{
    double relpos = 1.0 - note->ModifiedPosition / SeenDuration;
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
        { VGet(left + xadjust, SU_LANE_Y_AIRINDICATE, z), VGet(0, 0, -1), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.0f, role, 0.0f, 0.0f },
        { VGet(right + xadjust, SU_LANE_Y_AIRINDICATE, z), VGet(0, 0, -1), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 1.0f, role, 0.0f, 0.0f },
        { VGet(right, SU_LANE_Y_GROUND, z), VGet(0, 0, -1), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 1.0f, role + 0.5f, 0.0f, 0.0f },
        { VGet(left, SU_LANE_Y_GROUND, z), VGet(0, 0, -1), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.0f, role + 0.5f, 0.0f, 0.0f }
    };
    Prepare3DDrawCall();
    SetUseZBuffer3D(FALSE);
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawPolygonIndexed3D(vertices, 4, RectVertexIndices, 2, handle, TRUE);
}

void ScenePlayer::DrawHoldNotes(shared_ptr<SusDrawableNoteData> note)
{
    auto length = note->Length;
    auto slane = note->StartLane;
    double relpos = 1.0 - note->ModifiedPosition / SeenDuration;
    //中身だけ先に描画
    auto endpoint = note->ExtraData.back();
    double reltailpos = 1.0 - endpoint->ModifiedPosition / SeenDuration;
    SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
    DrawModiGraphF(
        slane * widthPerLane, laneBufferY * relpos,
        (slane + length) * widthPerLane, laneBufferY * relpos,
        (slane + length) * widthPerLane, laneBufferY * reltailpos,
        slane * widthPerLane, laneBufferY * reltailpos,
        imageHoldStrut->GetHandle(), TRUE
    );

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawTap(slane, length, relpos, imageHold->GetHandle());

    for (auto &ex : note->ExtraData) {
        if (ex->Type.test(SusNoteType::ExTap)) continue;
        double relendpos = 1.0 - ex->ModifiedPosition / SeenDuration;
        DrawTap(slane, length, relendpos, imageHold->GetHandle());
    }
}

void ScenePlayer::DrawSlideNotes(shared_ptr<SusDrawableNoteData> note)
{
    auto lastStep = note;
    auto lastStepRelativeY = 1.0 - lastStep->ModifiedPosition / SeenDuration;
    double segmentLength = 128.0;   // Buffer上での最小の長さ

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawTap(lastStep->StartLane, lastStep->Length, lastStepRelativeY, imageSlide->GetHandle());

    for (auto &slideElement : note->ExtraData) {
        if (slideElement->Type.test(SusNoteType::Control)) continue;
        if (slideElement->Type.test(SusNoteType::ExTap)) continue;
        double currentStepRelativeY = 1.0 - slideElement->ModifiedPosition / SeenDuration;
        auto &segmentPositions = curveData[slideElement];

        auto lastSegmentPosition = segmentPositions[0];
        double lastSegmentLength = lastStep->Length;
        double lastTimeInBlock = get<0>(lastSegmentPosition) / (slideElement->StartTime - lastStep->StartTime);
        auto lastSegmentRelativeY = 1.0 - lastStep->ModifiedPosition / SeenDuration;
        double currentExPosition = get<1>(lastStep->Timeline->GetRawDrawStateAt(CurrentTime));
        for (auto &segmentPosition : segmentPositions) {
            if (lastSegmentPosition == segmentPosition) continue;
            double currentTimeInBlock = get<0>(segmentPosition) / (slideElement->StartTime - lastStep->StartTime);
            double currentSegmentLength = (1.0 - currentTimeInBlock) * lastStep->Length + currentTimeInBlock * slideElement->Length;
            double segmentExPosition = (1.0 - currentTimeInBlock) * lastStep->ModifiedPosition + currentTimeInBlock * slideElement->ModifiedPosition;
            double currentSegmentRelativeY = 1.0 - segmentExPosition / SeenDuration;

            if (currentSegmentRelativeY < cullingLimit && lastSegmentRelativeY < cullingLimit) {
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
            }
            lastSegmentPosition = segmentPosition;
            lastSegmentLength = currentSegmentLength;
            lastSegmentRelativeY = currentSegmentRelativeY;
            lastTimeInBlock = currentTimeInBlock;
        }

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
        if (!slideElement->Type.test(SusNoteType::Tap))
            DrawTap(slideElement->StartLane, slideElement->Length, currentStepRelativeY, imageSlide->GetHandle());

        lastStep = slideElement;
        lastStepRelativeY = currentStepRelativeY;
    }
}

void ScenePlayer::DrawAirActionNotes(shared_ptr<SusDrawableNoteData> note)
{
    auto lastStep = note;
    auto lastStepRelativeY = 1.0 - lastStep->ModifiedPosition / SeenDuration;
    double segmentLength = 128.0;   // Buffer上での最小の長さ

    double aasz = (1.0 - lastStepRelativeY) * SU_LANE_Z_MAX + lastStepRelativeY * SU_LANE_Z_MIN;
    double cry = ((double)lastStep->StartLane + lastStep->Length / 2.0) / 16.0;
    double center = (1.0 - cry) * SU_LANE_X_MIN + cry * SU_LANE_X_MAX;
    VERTEX3D vertices[] = {
        { VGet(center - 10, SU_LANE_Y_GROUND, aasz), VGet(0, 0, -1), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.9375f, 1.0f, 1.0f, 0.0f },
        { VGet(center - 10, SU_LANE_Y_AIR, aasz), VGet(0, 0, -1), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.9375f, 0.0f, 0.0f, 0.0f },
        { VGet(center + 10, SU_LANE_Y_AIR, aasz), VGet(0, 0, -1), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 1.0000f, 0.0f, 0.0f, 0.0f },
        { VGet(center + 10, SU_LANE_Y_GROUND, aasz), VGet(0, 0, -1), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 1.0000f, 1.0f, 0.0f, 0.0f },
    };
    DrawPolygonIndexed3D(vertices, 4, RectVertexIndices, 2, imageAirAction->GetHandle(), TRUE);

    for (auto &slideElement : note->ExtraData) {
        if (slideElement->Type.test(SusNoteType::Control)) continue;
        if (slideElement->Type.test(SusNoteType::ExTap)) continue;
        double currentStepRelativeY = 1.0 - slideElement->ModifiedPosition / SeenDuration;
        auto &segmentPositions = curveData[slideElement];

        auto lastSegmentPosition = segmentPositions[0];
        double lastSegmentLength = lastStep->Length;
        double lastTimeInBlock = get<0>(lastSegmentPosition) / (slideElement->StartTime - lastStep->StartTime);
        auto lastSegmentRelativeY = 1.0 - lastStep->ModifiedPosition / SeenDuration;
        double currentExPosition = get<1>(lastStep->Timeline->GetRawDrawStateAt(CurrentTime));
        for (auto &segmentPosition : segmentPositions) {
            if (lastSegmentPosition == segmentPosition) continue;
            double currentTimeInBlock = get<0>(segmentPosition) / (slideElement->StartTime - lastStep->StartTime);
            double currentSegmentLength = (1.0 - currentTimeInBlock) * lastStep->Length + currentTimeInBlock * slideElement->Length;
            double segmentExPosition = (1.0 - currentTimeInBlock) * lastStep->ModifiedPosition + currentTimeInBlock * slideElement->ModifiedPosition;
            double currentSegmentRelativeY = 1.0 - segmentExPosition / SeenDuration;

            if (currentSegmentRelativeY < cullingLimit && lastSegmentRelativeY < cullingLimit) {
                SetUseZBuffer3D(TRUE);
                double back = (1.0 - currentSegmentRelativeY) * SU_LANE_Z_MAX + currentStepRelativeY * SU_LANE_Z_MIN;
                double front = (1.0 - lastSegmentRelativeY) * SU_LANE_Z_MAX + currentStepRelativeY * SU_LANE_Z_MIN;
                double backLeft = (get<1>(segmentPosition) - lastSegmentLength / 32.0);
                double backRight = (get<1>(segmentPosition) + lastSegmentLength / 32.0);
                double frontLeft = (get<1>(lastSegmentPosition) - lastSegmentLength / 32.0);
                double frontRight = (get<1>(lastSegmentPosition) + lastSegmentLength / 32.0);
                double pbl = (1.0 - backLeft) * SU_LANE_X_MIN + backLeft * SU_LANE_X_MAX;
                double pbr = (1.0 - backRight) * SU_LANE_X_MIN + backRight * SU_LANE_X_MAX;
                double pfl = (1.0 - frontLeft) * SU_LANE_X_MIN + frontLeft * SU_LANE_X_MAX;
                double pfr = (1.0 - frontRight) * SU_LANE_X_MIN + frontRight * SU_LANE_X_MAX;
                VERTEX3D vertices[] = {
                    { VGet(pfl, SU_LANE_Y_AIR, front), VGet(0, 0, -1), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.875f, 1.0f, 1.0f, 0.0f },
                    { VGet(pbl, SU_LANE_Y_AIR, back), VGet(0, 0, -1), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.875f, 0.0f, 0.0f, 0.0f },
                    { VGet(pbr, SU_LANE_Y_AIR, back), VGet(0, 0, -1), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.9375f, 0.0f, 0.0f, 0.0f },
                    { VGet(pfr, SU_LANE_Y_AIR, front), VGet(0, 0, -1), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.9375f, 1.0f, 0.0f, 0.0f },
                };
                DrawPolygonIndexed3D(vertices, 4, RectVertexIndices, 2, imageAirAction->GetHandle(), TRUE);

                vertices[0].pos.x = (1.0 - get<1>(lastSegmentPosition)) * SU_LANE_X_MIN + get<1>(lastSegmentPosition) * SU_LANE_X_MAX - 10;
                vertices[1].pos.x = (1.0 - get<1>(segmentPosition)) * SU_LANE_X_MIN + get<1>(segmentPosition) * SU_LANE_X_MAX - 10;
                vertices[2].pos.x = (1.0 - get<1>(segmentPosition)) * SU_LANE_X_MIN + get<1>(segmentPosition) * SU_LANE_X_MAX + 10;
                vertices[3].pos.x = (1.0 - get<1>(lastSegmentPosition)) * SU_LANE_X_MIN + get<1>(lastSegmentPosition) * SU_LANE_X_MAX + 10;
                vertices[0].u = 0.9375f; vertices[0].v = 1.0f;
                vertices[1].u = 0.9375f; vertices[1].v = 0.0f;
                vertices[2].u = 1.0000f; vertices[2].v = 0.0f;
                vertices[3].u = 1.0000f; vertices[3].v = 1.0f;
                DrawPolygonIndexed3D(vertices, 4, RectVertexIndices, 2, imageAirAction->GetHandle(), TRUE);
            }

            lastSegmentPosition = segmentPosition;
            lastSegmentLength = currentSegmentLength;
            lastSegmentRelativeY = currentSegmentRelativeY;
            lastTimeInBlock = currentTimeInBlock;
        }
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
        if (!slideElement->Type.test(SusNoteType::Tap)) {
            double atLeft = (slideElement->StartLane) / 16.0;
            double atRight = (slideElement->StartLane + slideElement->Length) / 16.0;
            double left = (1.0 - atLeft) * SU_LANE_X_MIN + atLeft * SU_LANE_X_MAX + 5;
            double right = (1.0 - atRight) * SU_LANE_X_MIN + atRight * SU_LANE_X_MAX - 5;
            double z = (1.0 - currentStepRelativeY) * SU_LANE_Z_MAX + currentStepRelativeY * SU_LANE_Z_MIN;
            auto color = GetColorU8(255, 255, 255, 255);
            VERTEX3D vertices[] = {
                //本体 上 手前
                /*
                llttttttrrSSSSCL
                llttttttrrSSSSCL
                --ffffff--SSSSCL
                --ffffff--SSSSCL
                DDDDDDDDDDSSSSCL
                DDDDDDDDDDSSSSCL
                DDDDDDDDDDSSSSCL
                DDDDDDDDDDSSSSCL
                左側面頂点位置
                9+--------+5
                |上のやつ|
                8+--------+4
                7+--------+3
                |  本1体 |
                6+----+---+2
                下 | の
                や | つ
                0  →手前
                */
                { VGet(left, SU_LANE_Y_GROUND, z), VGet(0, 0, -1), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.625f, 1.0f, 0.0f, 0.0f },
                { VGet(left, SU_LANE_Y_AIR, z), VGet(0, 0, -1), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.625f, 0.0f, 0.0f, 0.0f },
                { VGet(left, SU_LANE_Y_AIR, z - 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.0f, 0.25f, 0.0f, 0.0f },
                { VGet(left, SU_LANE_Y_AIR + 20, z - 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.125f, 0.25f, 0.0f, 0.0f },
                { VGet(left, SU_LANE_Y_AIR + 20, z - 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.208f, 1.0f, 0.0f, 0.0f },
                { VGet(left, SU_LANE_Y_AIR + 40, z - 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.208f, 0.5f, 0.0f, 0.0f },
                { VGet(left, SU_LANE_Y_AIR, z + 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.0f, 0.0f, 0.0f, 0.0f },
                { VGet(left, SU_LANE_Y_AIR + 20, z + 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.125f, 0.0f, 0.0f, 0.0f },
                { VGet(left, SU_LANE_Y_AIR + 20, z + 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.0f, 1.0f, 0.0f, 0.0f },
                { VGet(left, SU_LANE_Y_AIR + 40, z + 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.0f, 0.5f, 0.0f, 0.0f },

                { VGet(right, SU_LANE_Y_GROUND, z), VGet(0, 0, -1), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.875f, 1.0f, 0.0f, 0.0f },
                { VGet(right, SU_LANE_Y_AIR, z), VGet(0, 0, -1), GetColorU8(255, 255, 255, 255), GetColorU8(0, 0, 0, 0), 0.875f, 0.0f, 0.0f, 0.0f },
                { VGet(right, SU_LANE_Y_AIR, z - 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.625f, 0.25f, 0.0f, 0.0f },
                { VGet(right, SU_LANE_Y_AIR + 20, z - 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.5f, 0.25f, 0.0f, 0.0f },
                { VGet(right, SU_LANE_Y_AIR + 20, z - 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.625f, 1.0f, 0.0f, 0.0f },
                { VGet(right, SU_LANE_Y_AIR + 40, z - 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.625f, 0.5f, 0.0f, 0.0f },
                { VGet(right, SU_LANE_Y_AIR, z + 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.625f, 0.0f, 0.0f, 0.0f },
                { VGet(right, SU_LANE_Y_AIR + 20, z + 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.5f, 0.0f, 0.0f, 0.0f },
                { VGet(right, SU_LANE_Y_AIR + 20, z + 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.416f, 1.0f, 0.0f, 0.0f },
                { VGet(right, SU_LANE_Y_AIR + 40, z + 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.416f, 0.5f, 0.0f, 0.0f },

                { VGet(left, SU_LANE_Y_AIR, z - 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.125f, 0.5f, 0.0f, 0.0f },
                { VGet(right, SU_LANE_Y_AIR, z - 20), VGet(0, 0, -1), color, GetColorU8(0, 0, 0, 0), 0.5f, 0.5f, 0.0f, 0.0f },
            };
            uint16_t indices[] = {
                //下のやつ
                0, 1, 11,
                0, 11, 10,
                //本体
                //上
                3, 7, 17,
                3, 17, 13,
                //左
                6, 7, 3,
                6, 3, 2,
                //右
                12, 13, 17,
                12, 17, 16,
                //手前
                20, 3, 13,
                20, 13, 21,

                //へばりついてるの
                //手前
                4, 5, 15,
                4, 15, 14,
                //後ろ
                8, 9, 19,
                8, 19, 18,
                //左
                8, 9, 5,
                8, 5, 4,
                //右
                14, 15, 19,
                14, 19, 18,
            };
            SetUseZBuffer3D(TRUE);
            DrawPolygonIndexed3D(vertices, 22, indices + 6, 16, imageAirAction->GetHandle(), TRUE);
            SetUseZBuffer3D(FALSE);
            DrawPolygonIndexed3D(vertices, 22, indices, 2, imageAirAction->GetHandle(), TRUE);
            //DrawTap(slideElement->StartLane, slideElement->Length, currentStepRelativeY, imageAirAction->GetHandle());
        }

        lastStep = slideElement;
        lastStepRelativeY = currentStepRelativeY;
    }
}

void ScenePlayer::DrawTap(int lane, int length, double relpos, int handle)
{
    for (int i = 0; i < length * 2; i++) {
        int type = i ? (i == length * 2 - 1 ? 2 : 1) : 0;
        DrawRectRotaGraph3F(
            (lane * 2 + i) * widthPerLane / 2, laneBufferY * relpos,
            noteImageBlockX * type, (0),
            noteImageBlockX, noteImageBlockY,
            0, noteImageBlockY / 2,
            actualNoteScaleX, actualNoteScaleY, 0,
            handle, TRUE, FALSE);
    }
}

void ScenePlayer::DrawMeasureLines()
{
    int division = 8;
    for (int i = 1; i < division; i++) DrawLineAA(laneBufferX / division * i, 0, laneBufferX / division * i, laneBufferY * cullingLimit, GetColor(255, 255, 255), 3);

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