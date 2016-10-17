#pragma once
#include "EffectFunction.h"

class EffectData;
class EffectInstance;
class EffectEmitter;

enum EmitterRateType
{
    RateEmission,           //ジワー
    BurstEmission           //バシュ
};

enum EffectType
{
    LoopEffect,
    OneshotEffect
};


struct ParticleData
{
    //パーティクル全体情報の参照も使いまわすので若干アレ
    union
    {
        float X;
        double Rate;
    };
    union
    {
        float Y;
        double Time;
    };
    union
    {
        float Angle;
        EmitterRateType RateType;
    };
    
    float VelX;
    float VelY;
    float VelAngle;
    float AccX;
    float AccY;
    float AccAngle;

    union
    {
        float LifeLeft;
        EffectEmitter *Emitter;
    };
    
};

class EffectInstance
{
    friend class EffectBuilder;
    friend class EffectData;
protected:
    std::vector<std::list<ParticleData*>> particles;
    std::vector<int> imageIndices;
    EffectData *parent;

public:
    EffectInstance();
    ~EffectInstance();

    void Update(double delta);
    void DrawAll(std::function<void(const ParticleData&, int)> drawFunc);
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
    DistributionBase *InitX = 0, *InitY = 0;
    DistributionBase *InitVelX = 0, *InitVelY = 0;
    DistributionBase *InitAccX = 0, *InitAccY = 0;

    void FillDefault();
};

class EffectData final
{
private:
    void SetParticleData(EffectEmitter *emitter, ParticleData *data);

public:
    EffectData(std::string name);
    ~EffectData();

    std::string Name;
    EffectType Type;
    double LoopTime;
    std::vector<EffectEmitter*> Emitters;

    EffectInstance* Instantiate();
    void InitializeInstance(EffectInstance *instance);
    void UpdateInstance(EffectInstance *instance, double delta);
};