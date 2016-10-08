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

    return result;
}

// EffectInstance ------------------------

EffectInstance::EffectInstance()
{
}

EffectInstance::~EffectInstance()
{
}

void EffectInstance::Update(double delta)
{
}
