#pragma once
#include "EffectFunction.h"

enum EmitterRateType
{
    Loop,           //ジワー
    Oneshot,        //バシュ
};

class EffectEmitter
{
public:
    EffectEmitter();
    ~EffectEmitter();

    double Wait;
    EmitterRateType Type;
    DistributionBase *Rate = 0;
    DistributionBase *LifeTime = 0;
    DistributionBase *InitVelX = 0, *InitVelY = 0;
    DistributionBase *InitAccX = 0, *InitAccY = 0;

    void FillDefault();
};

class EffectData final
{
private:


public:
    EffectData(std::string name);
    ~EffectData();

    std::string Name;
    std::vector<EffectEmitter*> Emitters;
};