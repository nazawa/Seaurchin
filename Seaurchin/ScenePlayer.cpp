#include "ScenePlayer.h"
#include "ExecutionManager.h"

using namespace std;

static SSound *soundTap, *soundFlick;
static SImage *imageTap, *imageExTap, *imageFlick;

static uint16_t VertexIndices[] = { 0, 1, 3, 3, 1, 2 };
static VERTEX3D Vertices[] = {
    {
        VGet(-500, 0, 2000),
        VGet(0, 1, 0),
        GetColorU8(255, 255, 255, 255),
        GetColorU8(0, 0, 0, 0),
        0.0f, 1.0f,
        0.0f, 0.0f
    },
    {
        VGet(500, 0, 2000),
        VGet(0, 1, 0),
        GetColorU8(255, 255, 255, 255),
        GetColorU8(0, 0, 0, 0),
        1.0f, 1.0f,
        0.0f, 0.0f
    },
    {
        VGet(500, 0, 0),
        VGet(0, 1, 0),
        GetColorU8(255, 255, 255, 255),
        GetColorU8(0, 0, 0, 0),
        1.0f, 0.0f,
        0.0f, 0.0f
    },
    {
        VGet(-500, 0, 0),
        VGet(0, 1, 0),
        GetColorU8(255, 255, 255, 255),
        GetColorU8(0, 0, 0, 0),
        0.0f, 0.0f,
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
    DeleteGraph(hGroundBuffer);
    DeleteGraph(hAirBuffer);
}

void ScenePlayer::Draw()
{
    double time = GetPlayingTime() - metaInfo->WaveOffset;
    double duration = 1.00;
    double preced = 0.1;    //叩いた瞬間などの処理のために多く取る分 判定は上のtime基準
    vector<shared_ptr<SusDrawableNoteData>> seenData;

    BEGIN_DRAW_TRANSACTION(hGroundBuffer);
    ClearDrawScreen();
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawGraph(0, 0, resources["LaneGround"]->GetHandle(), TRUE);

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

    seenObjects = seenData.size();

    for (auto& note : seenData) {
        double relpos = (note->StartTime - time) / duration;
        auto length = note->Length;
        auto slane = note->StartLane;
        int handleToDraw = 0;

        if (relpos < 0 && !note->OnTheFlyData.test(NoteAttribute::Finished)) {
            note->OnTheFlyData.set(NoteAttribute::Finished);
            soundTap->Play();
        }

        if (note->Type.test(SusNoteType::Tap)) {
            handleToDraw = resources["Tap"]->GetHandle();
        } else if (note->Type.test(SusNoteType::ExTap)) {
            handleToDraw = resources["ExTap"]->GetHandle();
        } else if (note->Type.test(SusNoteType::Flick)) {
            handleToDraw = resources["Flick"]->GetHandle();
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
    FINISH_DRAW_TRANSACTION;

    SetUseLighting(FALSE);
    SetCameraPositionAndTarget_UpVecY(VGet(0, 500, -400), VGet(0, 0, 600));
    DrawPolygonIndexed3D(Vertices, 4, VertexIndices, 2, hGroundBuffer, TRUE);
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
    soundTap = dynamic_cast<SSound*>(resources["SoundTap"]);
    soundFlick = dynamic_cast<SSound*>(resources["SoundFlick"]);

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
