#include "EffectBuilder.h"
#include "Debug.h"
#include "Misc.h"

using namespace std;
using namespace sefx;

static constexpr auto hashstr = &crc_ccitt::checksum;

EffectBuilder::EffectBuilder(std::shared_ptr<std::mt19937> rnd)
{
    Random = rnd;
}

EffectBuilder::~EffectBuilder()
{
    for (auto& p : Effects) delete p.second;
}

void EffectBuilder::LoadFromFile(std::string fileName)
{
    string source, line;
    ifstream file;
    file.open(fileName, ios::in);
    if (!file) return;
    ostringstream oss;
    while (getline(file, line))
    {
        oss << line << endl;
    }
    file.close();
    source = oss.str();

    if (!ParseSource(source)) return;
}

bool EffectBuilder::ParseSource(std::string source)
{
    using boost::optional;
    using namespace boost::spirit;
    using namespace sefx;

    auto srcbegin = source.begin();
    auto srcend = source.end();
    string str; 
    vector<EffectTuple> effects;
    try
    {
        qi::phrase_parse(srcbegin, srcend, *EffectGrammar<string::iterator>(), qi::space, effects);
    }
    catch (exception e)
    {
        WriteDebugConsole(e.what());
        return false;
    }

    for (auto& efx : effects)
    {
        auto name = get<0>(efx);
        auto nfx = new EffectData(name);
        for (auto& eel : get<1>(efx))
        {
            // Effect / Dist
            if (get<0>(eel))
            {
                auto ep = get<0>(eel).get();
                switch (hashstr(ep.name.c_str()))
                {

                }
            }
            // Effect / String
            if (get<1>(eel))
            {
                auto ep = get<1>(eel).get();
                switch (hashstr(ep.name.c_str()))
                {
                case hashstr("type"):
                    if (ep.values[0] == "loop") nfx->Type = EffectType::LoopEffect;
                    if (ep.values[0] == "oneshot") nfx->Type = EffectType::OneshotEffect;
                    break;
                }
            }
            // Effect / Number
            if (get<2>(eel))
            {
                auto ep = get<2>(eel).get();
                switch (hashstr(ep.name.c_str()))
                {
                case hashstr("looptime"):
                    nfx->LoopTime = ep.values[0];
                    break;
                }
            }
            // Effect / Emitter
            if (get<3>(eel))
            {
                auto emitter = new EffectEmitter();
                ParseEmitter(emitter, get<3>(eel).get());
                emitter->FillDefault();
                nfx->Emitters.push_back(emitter);
            }
        }
        Effects[name] = nfx;
    }
    return true;
}

void EffectBuilder::ParseEmitter(EffectEmitter *emitter, const vector<sefx::EffectOptionals>& data)
{
    using boost::optional;
    using namespace boost::spirit;
    using namespace sefx;
    
    for (auto& d : data)
    {
        if (get<0>(d))ParseEmitterParameter(emitter, get<0>(d).get());
        if (get<1>(d))ParseEmitterParameter(emitter, get<1>(d).get());
        if (get<2>(d))ParseEmitterParameter(emitter, get<2>(d).get());
    }
}

void EffectBuilder::ParseEmitterParameter(EffectEmitter *emitter, const EffectParameter<EffectDistribution> &param)
{
    switch (hashstr(param.name.c_str()))
    {
    case hashstr("size"):
        break;
    case hashstr("color"):
        break;
    case hashstr("alpha"):
        break;

    case hashstr("burst"):
        emitter->Type = EmitterRateType::BurstEmission;
        emitter->Rate = GetDistribution(param.values[0]);
        break;
    case hashstr("rate"):
        emitter->Type = EmitterRateType::RateEmission;
        emitter->Rate = GetDistribution(param.values[0]);
        break;
    case hashstr("velocity"):
        if (param.values.size() != 2)
        {
            WriteDebugConsole("'velocity' Parameter Doesn't Match!\n");
            return;
        }
        emitter->InitVelX = GetDistribution(param.values[0]);
        emitter->InitVelY = GetDistribution(param.values[1]);
        break;
    case hashstr("accel"):
        if (param.values.size() != 2)
        {
            WriteDebugConsole("'accel' Parameter Doesn't Match!\n");
            return;
        }
        emitter->InitAccX = GetDistribution(param.values[0]);
        emitter->InitAccY = GetDistribution(param.values[1]);
        break;
    case hashstr("location"):
        if (param.values.size() != 2)
        {
            WriteDebugConsole("'location' Parameter Doesn't Match!\n");
            return;
        }
        emitter->InitX = GetDistribution(param.values[0]);
        emitter->InitY = GetDistribution(param.values[1]);
        break;
    case hashstr("lifetime"):
        emitter->LifeTime = GetDistribution(param.values[0]);
        break;

    }
}

void EffectBuilder::ParseEmitterParameter(EffectEmitter *emitter, const EffectParameter<string> &param)
{
    switch (hashstr(param.name.c_str()))
    {
    case hashstr("texture"):
        emitter->Name = param.values[0];
        break;
    }
}

void EffectBuilder::ParseEmitterParameter(EffectEmitter *emitter, const EffectParameter<double> &param)
{
    switch (hashstr(param.name.c_str()))
    {
    case hashstr("wait"):
        emitter->Wait = param.values[0];
        break;
    case hashstr("index"):
        emitter->ZIndex = (int)param.values[0];
        break;
    }
}

DistributionBase* EffectBuilder::GetDistribution(const sefx::EffectDistribution& dist)
{
    switch (hashstr(dist.name.c_str()))
    {
    case hashstr("fix"):
        return new DistributionFix(dist.parameters[0]);
    case hashstr("uniform"):
        return new DistributionUniform(Random, dist.parameters[0], dist.parameters[1]);
    case hashstr("normal"):
        return new DistributionNormal(Random, dist.parameters[0], dist.parameters[1]);
    }
    return nullptr;
}

