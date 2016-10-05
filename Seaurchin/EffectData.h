#pragma once

enum EffectType
{
    Loop,
    Oneshot,
};

class EffectDistribution
{
protected:
    std::vector<double> params;
public:
    virtual double Take() = 0;
};

class EffectEmitter
{
public:
    double wait;

};

class EffectData
{
private:

public:
    EffectData(std::string name);
    ~EffectData();

    std::string Name;
};