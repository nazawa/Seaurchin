#include "ScenePlayer.h"
#include "ScriptSprite.h"
#include "ExecutionManager.h"

using namespace std;

static SSound *soundTap, *soundExTap, *soundFlick;
static SImage *imageTap, *imageExTap, *imageFlick, *imageHold, *imageHoldStrut, *imageSlide, *imageSlideStrut;
static SFont *fontCombo;
static STextSprite *textCombo;

static uint16_t VertexIndices[] = { 0, 1, 3, 3, 1, 2 };
static VERTEX3D Vertices[] = {
    {
        VGet(-500, 0, 2000),
        VGet(0, 1, 0),
        GetColorU8(255, 255, 255, 255),
        GetColorU8(0, 0, 0, 0),
        0.0f, 0.0f,
        0.0f, 0.0f
    },
    {
        VGet(500, 0, 2000),
        VGet(0, 1, 0),
        GetColorU8(255, 255, 255, 255),
        GetColorU8(0, 0, 0, 0),
        1.0f, 0.0f,
        0.0f, 0.0f
    },
    {
        VGet(500, 0, 0),
        VGet(0, 1, 0),
        GetColorU8(255, 255, 255, 255),
        GetColorU8(0, 0, 0, 0),
        1.0f, 1.0f,
        0.0f, 0.0f
    },
    {
        VGet(-500, 0, 0),
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
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void Initialize()", asMETHOD(ScenePlayer, Initialize), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void SetResource(const string &in, " SU_IF_IMAGE "@)", asMETHOD(ScenePlayer, SetPlayerResource), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void SetResource(const string &in, " SU_IF_FONT "@)", asMETHOD(ScenePlayer, SetPlayerResource), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void SetResource(const string &in, " SU_IF_SOUND "@)", asMETHOD(ScenePlayer, SetPlayerResource), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void DrawLanes()", asMETHOD(ScenePlayer, Draw), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void Play()", asMETHOD(ScenePlayer, Play), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "double GetCurrentTime()", asMETHOD(ScenePlayer, GetPlayingTime), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "int GetSeenObjectsCount()", asMETHOD(ScenePlayer, GetSeenObjectsCount), asCALL_THISCALL);
}


ScenePlayer::ScenePlayer(ExecutionManager *exm) : manager(exm)
{

}

void ScenePlayer::AddRef()
{
    reference++;
}

void ScenePlayer::Release()
{
    if (--reference == 0) {
        Finalize();
        delete this;
    }
}

void ScenePlayer::Finalize()
{
    for (auto& res : resources) res.second->Release();
    manager->GetSoundManagerUnsafe()->Stop(bgmStream);
    manager->GetSoundManagerUnsafe()->ReleaseSound(bgmStream);

    fontCombo->Release();
    DeleteGraph(hGroundBuffer);
    DeleteGraph(hAirBuffer);
}

void ScenePlayer::Draw()
{
    double time = GetPlayingTime() - metaInfo->WaveOffset;
    double duration = 1.00;
    double preced = 0.1;    //叩いた瞬間などの処理のために多く取る分 判定は上のtime基準
    int division = 8;

    CalculateNotes(time, duration, preced);
    ProcessScore(time, duration, preced);
    ostringstream combo;
    combo << setw(4) << comboCount;
    textCombo->set_Text(combo.str());

    BEGIN_DRAW_TRANSACTION(hGroundBuffer);
    ClearDrawScreen();
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

    DrawGraph(0, 0, resources["LaneGround"]->GetHandle(), TRUE);
    for (int i = 1; i < division; i++) DrawLineAA(1024 / division * i, 0, 1024 / division * i, 2048, GetColor(255, 255, 255), 2);
    textCombo->Draw();

    DrawShortNotes(time, duration, preced);
    DrawLongNotes(time, duration, preced);
    FINISH_DRAW_TRANSACTION;

    SetUseLighting(FALSE);
    SetCameraPositionAndTarget_UpVecY(VGet(0, 500, -400), VGet(0, 0, 600));
    DrawPolygonIndexed3D(Vertices, 4, VertexIndices, 2, hGroundBuffer, TRUE);
}

void ScenePlayer::CalculateNotes(double time, double duration, double preced)
{
    seenData.clear();
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

void ScenePlayer::DrawShortNotes(double time, double duration, double preced)
{
    for (auto& note : seenData) {
        double relpos = 1.0 - (note->StartTime - time) / duration;
        auto length = note->Length;
        auto slane = note->StartLane;
        int handleToDraw = 0;

        if (note->Type.test(SusNoteType::Tap)) {
            handleToDraw = imageTap->GetHandle();
        } else if (note->Type.test(SusNoteType::ExTap)) {
            handleToDraw = imageExTap->GetHandle();
        } else if (note->Type.test(SusNoteType::Flick)) {
            handleToDraw = imageFlick->GetHandle();
        } else {
            //暫定的に
            continue;
        }

        //64*3 x 64 を描画するから1/2でやる必要がある
        for (int i = 0; i < length * 2; i++) {
            int type = i ? (i == length * 2 - 1 ? 2 : 1) : 0;
            DrawRectRotaGraph3F((slane * 2 + i) * 32.0f, 2048.0f * relpos, 64 * type, (0), 64, 64, 0, 32.0f, 0.5f, 0.5f, 0, handleToDraw, TRUE, FALSE);
        }
    }
}

void ScenePlayer::DrawLongNotes(double time, double duration, double preced)
{
    for (auto& note : seenData) {
        double relpos = 1.0 - (note->StartTime - time) / duration;
        auto length = note->Length;
        auto slane = note->StartLane;

        if (note->Type.test(SusNoteType::Hold)) {
            double relendpos = 1.0 - (note->ExtraData[0]->StartTime - time) / duration;
            DrawModiGraphF(
                slane * 64.0f, 2048.0f * relpos,
                (slane + length) * 64.0f, 2048.0f * relpos,
                (slane + length) * 64.0f, 2048.0f * relendpos,
                slane * 64.0f, 2048.0f * relendpos,
                imageHoldStrut->GetHandle(), TRUE
            );
            for (int i = 0; i < length * 2; i++) {
                int type = i ? (i == length * 2 - 1 ? 2 : 1) : 0;
                DrawRectRotaGraph3F((slane * 2 + i) * 32.0f, 2048.0f * relpos, 64 * type, (0), 64, 64, 0, 32.0f, 0.5f, 0.5f, 0, imageHold->GetHandle(), TRUE, FALSE);
                DrawRectRotaGraph3F((slane * 2 + i) * 32.0f, 2048.0f * relendpos, 64 * type, (0), 64, 64, 0, 32.0f, 0.5f, 0.5f, 0, imageHold->GetHandle(), TRUE, FALSE);
            }
        } else if (note->Type.test(SusNoteType::Slide)) {
            auto last = note;
            auto lastpos = 1.0 - (note->StartTime - time) / duration;
            //現在の描画は制御点考慮なし
            for (int i = 0; i < note->Length * 2; i++) {
                int type = i ? (i == note->Length * 2 - 1 ? 2 : 1) : 0;
                DrawRectRotaGraph3F((note->StartLane * 2 + i) * 32.0f, 2048.0f * relpos, 64 * type, (0), 64, 64, 0, 32.0f, 0.5f, 0.5f, 0, imageSlide->GetHandle(), TRUE, FALSE);
            }
            for (auto &ex : note->ExtraData) {
                double relendpos = 1.0 - (ex->StartTime - time) / duration;
                DrawModiGraphF(
                    last->StartLane * 64.0f, 2048.0f * lastpos,
                    (last->StartLane + last->Length) * 64.0f, 2048.0f * lastpos,
                    (ex->StartLane + ex->Length) * 64.0f, 2048.0f * relendpos,
                    ex->StartLane * 64.0f, 2048.0f * relendpos,
                    imageSlideStrut->GetHandle(), TRUE
                );
                for (int i = 0; i < ex->Length * 2; i++) {
                    int type = i ? (i == ex->Length * 2 - 1 ? 2 : 1) : 0;
                    DrawRectRotaGraph3F((ex->StartLane * 2 + i) * 32.0f, 2048.0f * relendpos, 64 * type, (0), 64, 64, 0, 32.0f, 0.5f, 0.5f, 0, imageSlide->GetHandle(), TRUE, FALSE);
                }
                last = ex;
                lastpos = relendpos;
            }
        } else if (note->Type.test(SusNoteType::AirAction)) {

        } else {
            continue;
        }
    }
}

void ScenePlayer::ProcessSound(double time, double duration, double preced)
{
    //役目不明
}

void ScenePlayer::ProcessScore(double time, double duration, double preced)
{
    for (auto& note : seenData) {
        double relpos = (note->StartTime - time) / duration;
        if (relpos >= 0 || (note->OnTheFlyData.test(NoteAttribute::Finished) && note->ExtraData.size() == 0)) continue;
        
        if (note->Type.test(SusNoteType::Flick)) {
            soundFlick->Play();
            comboCount++;
            note->OnTheFlyData.set(NoteAttribute::Finished);
        } else if (note->Type.test(SusNoteType::ExTap)) {
            soundExTap->Play();
            comboCount++;
            note->OnTheFlyData.set(NoteAttribute::Finished);
        } else if (note->Type.test(SusNoteType::Tap)
            || note->Type.test(SusNoteType::Air)) {
            soundTap->Play();
            comboCount++;
            note->OnTheFlyData.set(NoteAttribute::Finished);
        } else if (note->Type.test(SusNoteType::Start)) {
            if (!note->OnTheFlyData.test(NoteAttribute::Finished)) {
                soundTap->Play();
                comboCount++;
                note->OnTheFlyData.set(NoteAttribute::Finished);
            }
            for (auto& ex : note->ExtraData) {
                relpos = (ex->StartTime - time) / duration;
                if (relpos < 0 && !ex->OnTheFlyData.test(NoteAttribute::Finished)) {
                    soundTap->Play();
                    comboCount++;
                    ex->OnTheFlyData.set(NoteAttribute::Finished);
                }
            }
        }
        
    }
}

// スクリプト側から呼べるやつら

void ScenePlayer::Initialize()
{
    analyzer = make_unique<SusAnalyzer>(192);
    metaInfo = manager->GetMusicsManager()->Selected;

    hGroundBuffer = MakeScreen(1024, 2048, TRUE);
    hAirBuffer = MakeScreen(1024, 2048, TRUE);

    analyzer->LoadFromFile(metaInfo->Path.string().c_str());
    analyzer->RenderScoreData(data);

    auto file = metaInfo->WavePath;
    bgmStream = manager->GetSoundManagerUnsafe()->LoadStreamFromFile(file.string().c_str());
}

void ScenePlayer::SetPlayerResource(const string & name, SResource * resource)
{
    if (resources.find(name) != resources.end()) resources[name]->Release();
    resources[name] = resource;
}


void ScenePlayer::Play()
{
    imageTap = dynamic_cast<SImage*>(resources["Tap"]);
    imageExTap = dynamic_cast<SImage*>(resources["ExTap"]);
    imageFlick = dynamic_cast<SImage*>(resources["Flick"]);
    imageHold = dynamic_cast<SImage*>(resources["Hold"]);
    imageHoldStrut = dynamic_cast<SImage*>(resources["HoldStrut"]);
    imageSlide = dynamic_cast<SImage*>(resources["Slide"]);
    imageSlideStrut = dynamic_cast<SImage*>(resources["SlideStrut"]);

    soundTap = dynamic_cast<SSound*>(resources["SoundTap"]);
    soundExTap = dynamic_cast<SSound*>(resources["SoundExTap"]);
    soundFlick = dynamic_cast<SSound*>(resources["SoundFlick"]);
    fontCombo = dynamic_cast<SFont*>(resources["FontCombo"]);

    fontCombo->AddRef();
    textCombo = STextSprite::Factory(fontCombo, "0000");
    textCombo->Apply("y:1536, scaleX:8, scaleY:8");

    manager->GetSoundManagerUnsafe()->Play(bgmStream);
}

double ScenePlayer::GetPlayingTime()
{
    return manager->GetSoundManagerUnsafe()->GetPosition(bgmStream);
}

int ScenePlayer::GetSeenObjectsCount()
{
    return seenObjects;
}
