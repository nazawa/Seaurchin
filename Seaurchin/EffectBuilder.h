#pragma once

#include "EffectData.h"
#include "EffectGrammar.h"
//‚à‚¤‚Ç‚¤‚É‚à‚È‚ñ‚Ë‚¦‚Á‚Ä‚Ì
using namespace boost::spirit;

class EffectBuilder final
{
private:
    std::shared_ptr<std::mt19937> Random;
    
    std::unordered_map<std::string, EffectData*> Effects;
    void ParseEmitter(EffectEmitter *emitter, const std::vector<sefx::EffectOptionals> &data);
    void ParseEmitterParameter(EffectEmitter *emitter, const sefx::EffectParameter<sefx::EffectDistribution> &param);
    void ParseEmitterParameter(EffectEmitter *emitter, const sefx::EffectParameter<std::string> &param);
    void ParseEmitterParameter(EffectEmitter *emitter, const sefx::EffectParameter<double> &param);
    DistributionBase* GetDistribution(const sefx::EffectDistribution &dist);

public:
    EffectBuilder(std::shared_ptr<std::mt19937> rnd);
    ~EffectBuilder();

    void LoadFromFile(const std::wstring &fileName);
    bool ParseSource(const std::string &source);
};

