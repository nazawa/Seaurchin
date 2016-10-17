#include "EffectData.h"

using namespace std;

EffectEmitter::EffectEmitter()
{
}

EffectEmitter::~EffectEmitter()
{
    if (Rate) delete Rate;
    if (LifeTime) delete LifeTime;
    if (InitVelX)
    {
        delete InitVelX;
        delete InitVelY;
    }
    if (InitAccX)
    {
        delete InitAccX;
        delete InitAccY;
    }
    if (InitX)
    {
        delete InitX;
        delete InitY;
    }
}

void EffectEmitter::FillDefault()
{
    if (!Rate) Rate = new DistributionFix(1);
    if (!LifeTime) LifeTime = new DistributionFix(1);

    if (!InitVelX)
    {
        //これ同じヤツ参照させた方がいいと思うんだよな
        InitVelX = new DistributionFix(0);
        InitVelY = new DistributionFix(0);
    }
    if (!InitAccX)
    {
        InitAccX = new DistributionFix(0);
        InitAccY = new DistributionFix(0);
    }
    if (!InitX)
    {
        InitX = new DistributionFix(0);
        InitY = new DistributionFix(0);
    }
}

// EffectData ----------------------------

EffectData::EffectData(std::string name)
{
    Name = name;
    Type = EffectType::LoopEffect;
}

EffectData::~EffectData()
{
    for (auto& em : Emitters) delete em;
}

EffectInstance* EffectData::Instantiate()
{
    auto result = new EffectInstance();
    sort(Emitters.begin(), Emitters.end(), [](EffectEmitter *a, EffectEmitter *b) { return a->ZIndex - b->ZIndex; });
    InitializeInstance(result);
    return result;
}

void EffectData::InitializeInstance(EffectInstance *instance)
{
    instance->parent = this;
    for (auto &em : Emitters)
    {
        list<ParticleData*> pr;
        //先頭にエミッタ情報を入れとく
        ParticleData *eminfo = new ParticleData();
        eminfo->Emitter = em;               //親エミッタ
        eminfo->RateType = em->Type;        //発射タイプ
        eminfo->X = em->Rate->Take();       //レート
        eminfo->Y = 0.0;                    //次の奴発車までの時間
        pr.push_back(eminfo);
        
        switch (em->Type)
        {
        case EmitterRateType::BurstEmission:
        {
            int num = (int)max(0, em->Rate->Take());
            for (int i = 0; i < num; i++)
            {
                ParticleData *pd = new ParticleData();
                SetParticleData(em, pd);
                pr.push_back(pd);
            }
            break;
        }
        case EmitterRateType::RateEmission:
            //なにもし(ないです)
            break;
        }
        //moveが最適化されるかどうかは知らん
        instance->particles.push_back(move(pr));
    }
}

void EffectData::SetParticleData(EffectEmitter *emitter, ParticleData *data)
{
    //角度関係は未実装ですね
    data->X = emitter->InitX->Take();
    data->Y = emitter->InitY->Take();
    data->VelX = emitter->InitVelX->Take();
    data->VelY = emitter->InitVelY->Take();
    data->AccX = emitter->InitAccX->Take();
    data->AccY = emitter->InitAccY->Take();
    data->LifeLeft = emitter->LifeTime->Take();
}

void EffectData::UpdateInstance(EffectInstance *instance, double delta)
{
    for (auto& pl : instance->particles)
    {
        auto i = pl.begin();
        auto ei = *(i++);
        while (i != pl.end())
        {
            auto ii = *i;
            ii->VelX += ii->AccX * delta;
            ii->VelY += ii->AccY * delta;
            ii->X += ii->VelX * delta;
            ii->Y += ii->VelY * delta;
            ii->LifeLeft -= delta;

            if (ii->LifeLeft <= 0)
            {
                i = pl.erase(i);
            }
            else
            {
                i++;
            }
        }
        if (ei->VelX > 0)
        {
            //Rate追加
            int num = 0;
            ei->Y += delta;
            ei->Y -= (num = (int)(ei->X * ei->Y)) / ei->X;
            for (int i = 0; i < num; i++)
            {
                ParticleData *pd = new ParticleData();
                SetParticleData(ei->Emitter, pd);
                pl.push_back(pd);
            }
        }
    }
}

// EffectInstance ------------------------

EffectInstance::EffectInstance()
{
}

EffectInstance::~EffectInstance()
{
    for (auto& pl : particles)
    {
        auto i = pl.begin();
        while (i != pl.end())
        {
            delete *i;
            i = pl.erase(i);
        }
    }
}

void EffectInstance::Update(double delta)
{
    parent->UpdateInstance(this, delta);
}

void EffectInstance::DrawAll(std::function<void(const ParticleData&, int)> drawFunc)
{
    for (int i = 0; i < particles.size(); i++)
    {
        for (auto &pi : particles[i]) drawFunc(*pi, i);
    }
}
