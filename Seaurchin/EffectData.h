#pragma once
#include "EffectFunction.h"

struct ParticleData
{
    float X;
    float Y;
    float Angle;
    float VelX;
    float VelY;
    float VelAngle;
    float AccX;
    float AccY;
    float AccAngle;
};

class EffectInstance
{
public:
    EffectInstance();
    ~EffectInstance();
};

enum EmitterRateType
{
    RateEmission,           //ジワー
    BurstEmission           //バシュ
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

enum EffectType
{
    LoopEffect,
    OneshotEffect
};

class EffectData final
{
private:


public:
    EffectData(std::string name);
    ~EffectData();

    std::string Name;
    EffectType Type;
    double LoopTime;
    std::vector<EffectEmitter*> Emitters;

    std::shared_ptr<EffectInstance> Instantiate();
};