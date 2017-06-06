#include "ScenePlayer.h"
#include "ExecutionManager.h"

using namespace std;

void RegisterPlayerScene(ExecutionManager * exm)
{
    auto engine = exm->GetScriptInterfaceUnsafe()->GetEngine();

    engine->RegisterObjectType(SU_IF_SCENE_PLAYER, 0, asOBJ_REF);
    engine->RegisterObjectBehaviour(SU_IF_SCENE_PLAYER, asBEHAVE_ADDREF, "void f()", asMETHOD(ScenePlayer, AddRef), asCALL_THISCALL);
    engine->RegisterObjectBehaviour(SU_IF_SCENE_PLAYER, asBEHAVE_RELEASE, "void f()", asMETHOD(ScenePlayer, Release), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void Initialize()", asMETHOD(ScenePlayer, Initialize), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void SetResource(const string &in, " SU_IF_IMAGE "@)", asMETHOD(ScenePlayer, SetPlayerResource), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void DrawLanes()", asMETHOD(ScenePlayer, Draw), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "void Play()", asMETHOD(ScenePlayer, Play), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "double GetCurrentTime()", asMETHOD(ScenePlayer, GetPlayingTime), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SCENE_PLAYER, "int GetSeenObjectsCount()", asMETHOD(ScenePlayer, GetSeenObjectsCount), asCALL_THISCALL);
}


void ScenePlayer::AddRef()
{
    reference++;
}

void ScenePlayer::Release()
{
    if (!--reference) {
        Finalize();
        delete this;
    }
}

void ScenePlayer::Initialize()
{
    hGroundBuffer = MakeScreen(1024, 2048, TRUE);
    hAirBuffer = MakeScreen(1024, 2048, TRUE);
    manager->GetMusicsManager()->RenderSelectedScore(data);
    auto meta = manager->GetMusicsManager()->Selected;
    auto file = meta->WavePath;
    bgmStream = manager->GetSoundManagerUnsafe()->LoadStreamFromFile(file.string().c_str());
}

void ScenePlayer::SetPlayerResource(const string & name, SResource * resource)
{
    if (resources.find(name) != resources.end()) resources[name]->Release();
    resources[name] = resource;
}

void ScenePlayer::Finalize()
{
    DeleteGraph(hGroundBuffer);
    DeleteGraph(hAirBuffer);
}

void ScenePlayer::Draw()
{
    double time = GetPlayingTime();
    double duration = 1.00;
    vector<SusDrawableNoteData> seenData;

    BEGIN_DRAW_TRANSACTION(hGroundBuffer);
    ClearDrawScreen();
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
    DrawGraph(0, 0, resources["LaneGround"]->GetHandle(), TRUE);

    copy_if(data.begin(), data.end(), back_inserter(seenData), [&](SusDrawableNoteData n) {
        if (n.Type.to_ulong() & 0b0000000011100000) {
            // ロング
            return (time <= n.StartTime && n.StartTime <= time + duration)
                || (time <= n.StartTime + n.Duration && n.StartTime + n.Duration <= time + duration)
                || (n.StartTime <= time && time + duration <= n.StartTime + n.Duration);
        } else {
            // ショート
            return (time <= n.StartTime && n.StartTime <= time + duration);
        }
    });

    seenObjects = seenData.size();

    for (auto& note : seenData) {
        double relpos = (note.StartTime - time) / duration;
        auto length = note.Length;
        auto slane = note.StartLane;
        int handleToDraw = 0;

        if (note.Type.test(SusNoteType::Tap)) {
            handleToDraw = resources["Tap"]->GetHandle();
        } else if (note.Type.test(SusNoteType::ExTap)) {
            handleToDraw = resources["ExTap"]->GetHandle();
        } else if (note.Type.test(SusNoteType::Flick)) {
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
    // DrawModiGraph(0, 0, 1024, 0, 1024, 2048, 0, 2048, resources["Tap"]->GetHandle(), TRUE);
    FINISH_DRAW_TRANSACTION;

    // 正しい方向は右回り
    /*
    0---14
    |  / |
    | /  |
    |/   |
    23---5
    */


    VERTEX3D Vertices[4] = { 0 };
    Vertices[0].pos = VGet(-500, 0, 2000);
    Vertices[0].u = 0.0f;
    Vertices[0].v = 1.0f;
    Vertices[0].norm = VGet(0, 1, 0);
    Vertices[0].dif = GetColorU8(255, 255, 255, 255);

    Vertices[1].pos = VGet(500, 0, 2000);
    Vertices[1].u = 1.0f;
    Vertices[1].v = 1.0f;
    Vertices[1].norm = VGet(0, 1, 0);
    Vertices[1].dif = GetColorU8(255, 255, 255, 255);

    Vertices[2].pos = VGet(500, 0, 0);
    Vertices[2].u = 1.0f;
    Vertices[2].v = 0.0f;
    Vertices[2].norm = VGet(0, 1, 0);
    Vertices[2].dif = GetColorU8(255, 255, 255, 255);

    Vertices[3].pos = VGet(-500, 0, 0);
    Vertices[3].u = 0.0f;
    Vertices[3].v = 0.0f;
    Vertices[3].norm = VGet(0, 1, 0);
    Vertices[3].dif = GetColorU8(255, 255, 255, 255);

    uint16_t indices[] = { 0, 1, 3, 3, 1, 2 };
    //ClearDrawScreen();
    SetUseLighting(FALSE);
    SetCameraPositionAndTarget_UpVecY(VGet(0, 500, -400), VGet(0, 0, 600));
    DrawPolygonIndexed3D(Vertices, 4, indices, 2, hGroundBuffer, TRUE);
}

bool ScenePlayer::IsDead()
{
    return false;
}


void ScenePlayer::Play()
{
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
