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
}

void EffectEmitter::FillDefault()
{
    if (!Rate) Rate = new DistributionFix(1);
    if (!LifeTime) LifeTime = new DistributionFix(1);
    if (!InitVelX)
    {
        //Ç±ÇÍìØÇ∂ÉÑÉcéQè∆Ç≥ÇπÇΩï˚Ç™Ç¢Ç¢Ç∆évÇ§ÇÒÇæÇÊÇ»
        InitVelX = new DistributionFix(0);
        InitVelY = new DistributionFix(0);
    }
    if (!InitAccX)
    {
        InitAccX = new DistributionFix(0);
        InitAccY = new DistributionFix(0);
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

std::shared_ptr<EffectInstance> EffectData::Instantiate()
{
    auto result = std::shared_ptr<EffectInstance>(new EffectInstance());
    sort(Emitters.begin(), Emitters.end(), [](EffectEmitter *a, EffectEmitter *b) { return a->ZIndex - b->ZIndex; });
    
    for (auto &em : Emitters)
    {
        
    }

    return result;
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
    for (auto& pl : particles)
    {
        auto i = pl.begin();
        while (i != pl.end())
        {
            (*i)->X += (*i)->VelX * delta;
            (*i)->Y += (*i)->VelY * delta;
            (*i)->Angle += (*i)->VelAngle * delta;
            (*i)->VelX += (*i)->AccX * delta;
            (*i)->VelY += (*i)->AccY * delta;
            (*i)->VelAngle += (*i)->AccAngle * delta;
            (*i)->LifeLeft -= delta;
            if ((*i)->LifeLeft < 0)
            {
                delete *i;
                i = pl.erase(i);
            }
            else
            {
                i++;
            }
        }
    }
}

void EffectInstance::DrawAll(std::function<void(const ParticleData&, int)> drawFunc)
{
    for (int i = 0; i < particles.size(); i++)
    {
        for (auto &pi : particles[i]) drawFunc(*pi, imageIndices[i]);
    }
}
