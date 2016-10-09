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
    float LifeLeft;
};

class EffectInstance
{
protected:
    std::vector<std::list<ParticleData*>> particles;
    std::vector<int> imageIndices;

public:
    EffectInstance();
    ~EffectInstance();

    void Update(double delta);
    void DrawAll(std::function<void(const ParticleData&, int)> drawFunc);
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

    int ZIndex;
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