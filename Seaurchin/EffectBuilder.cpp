#include "EffectBuilder.h"
#include "Debug.h"

using namespace std;

EffectBuilder::EffectBuilder()
{
}

EffectBuilder::~EffectBuilder()
{
    for (auto& p : effects) delete p.second;
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

            }
            // Effect / String
            if (get<1>(eel))
            {

            }
            // Effect / Number
            if (get<2>(eel))
            {

            }
            // Effect / Emitter
            if (get<3>(eel))
            {

            }
        }
    }

    return false;
}