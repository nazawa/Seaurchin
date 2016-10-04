#pragma once

#include "EffectData.h"

//‚à‚¤‚Ç‚¤‚É‚à‚È‚ñ‚Ë‚¦‚Á‚Ä‚Ì
using namespace boost::spirit;

class EffectBuilder final
{
private:

    bool ParseSource(std::string source, EffectData *output);

public:
    EffectData* LoadFromFile(std::string fileName);
};

template<typename Iter>
struct EffectGrammar : qi::grammar<Iter, std::tuple<std::string, int>, qi::space_type>
{
    qi::rule<Iter, std::tuple<std::string, int>(), qi::space_type> rEffect;
    qi::rule<Iter, std::vector<double>(), qi::space_type> rDist;
    qi::rule<Iter, std::string(), qi::space_type> rType;
    

    EffectGrammar() : EffectGrammar::base_type(rEffect)
    {
        rType = qi::lit("type") >> 
    }
};