#pragma once

#include "EffectData.h"

//‚à‚¤‚Ç‚¤‚É‚à‚È‚ñ‚Ë‚¦‚Á‚Ä‚Ì
using namespace boost::spirit;

class EffectBuilder final
{
private:
    std::unordered_map<std::string, EffectData*> effects;

public:
    EffectBuilder();
    ~EffectBuilder();

    void LoadFromFile(std::string fileName);
    bool ParseSource(std::string source);
};

#include "EffectGrammar.h"