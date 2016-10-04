#include "EffectBuilder.h"

using namespace std;

EffectData * EffectBuilder::LoadFromFile(std::string fileName)
{
    string source, line;
    ifstream file;
    file.open(fileName, ios::in);
    if (!file) return nullptr;
    ostringstream oss;
    while (getline(file, line))
    {
        oss << line << endl;
    }
    file.close();
    source = oss.str();

    auto result = new EffectData();
    if (!ParseSource(source, result))
    {

        return nullptr;
    }
    return result;
}

bool EffectBuilder::ParseSource(std::string source, EffectData *output)
{
    using namespace boost::spirit;

    auto srcbegin = source.begin();
    auto srcend = source.end();

    auto pattern = qi::lit("effect");

    qi::phrase_parse(srcbegin, srcend, pattern, qi::space);

    return false;
}