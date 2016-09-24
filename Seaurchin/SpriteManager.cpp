#include "SpriteManager.h"

using namespace std;
using namespace boost::xpressive;

sregex SpriteManager::srparam = (s1 = +_w) >> ':' >> (s2 = +_w | (+_d >> !('.' >> +_d)));
sregex SpriteManager::srmove = bos >> (s1 = +_w) >> !('(' >> (s2 = SpriteManager::srparam >> *(',' >> SpriteManager::srparam)) >> ')') >> eos;

unordered_map<string, function<bool(shared_ptr<Sprite>, SpriteManager::Mover&, double)>> SpriteManager::actions =
{
    {"move_to", SpriteManager::ActionMoveTo},
    {"move_by", SpriteManager::ActionMoveBy}
};

unordered_map<string, Easing::EasingFunction> SpriteManager::easings =
{
    { "linear", Easing::Linear },
    { "in_quad", Easing::InQuad },
    { "out_quad", Easing::OutQuad },
    { "inout_quad", Easing::InOutQuad },
    { "in_cubic", Easing::InCubic },
    { "out_cubic", Easing::OutCubic },
    { "inout_cubic", Easing::InOutCubic },
    { "in_quart", Easing::InQuart },
    { "out_quart", Easing::OutQuart },
    { "inout_quart", Easing::InOutQuart },
    { "in_quint", Easing::InQuint },
    { "out_quint", Easing::OutQuint },
    { "inout_quint", Easing::InOutQuint },
    { "in_sin", Easing::InSine },
    { "out_sin", Easing::OutSine },
    { "inout_sin", Easing::InOutSine },
    { "in_expo", Easing::InExpo },
    { "out_expo", Easing::OutExpo },
    { "inout_expo", Easing::InOutExpo },
    { "in_circle", Easing::InCircle },
    { "out_circle", Easing::OutCircle },
    { "inout_circle", Easing::InOutCircle },
    { "in_elastic", Easing::InElastic },
    { "out_elastic", Easing::OutElastic },
    { "inout_elastic", Easing::InOutElastic },
    { "in_back", Easing::InBack },
    { "out_back", Easing::OutBack },
    { "inout_back", Easing::InOutBack },
    { "in_bounce", Easing::InBounce },
    { "out_bounce", Easing::OutBounce },
    { "inout_bounce", Easing::InOutBounce }
};

SpriteManager::SpriteManager()
{
}

void SpriteManager::AddMove(shared_ptr<Sprite> sprite, std::string move)
{
    using namespace boost::algorithm;
    constexpr auto hash = &crc_ccitt::checksum;
    smatch match;
    move.erase(remove(move.begin(), move.end(), ' '), move.end());
    bool m = regex_match(move, match, srmove);
    if (!m) return;

    Mover *mover = new Mover{ 0 };
    mover->Function = Easing::Linear;
    auto action = actions[match[s1].str()];
    vector<string> params;
    split(params, match[s2].str(), is_any_of(","));
    for (const auto& s : params)
    {
        regex_match(s, match, srparam);
        auto pname = match[s1].str();
        auto pval = match[s2].str();
        switch (hash(pname.c_str()))
        {
        case hash("x"):
        case hash("r"):
            mover->X = atof(pval.c_str());
            break;
        case hash("y"):
        case hash("g"):
            mover->Y = atof(pval.c_str());
            break;
        case hash("z"):
        case hash("b"):
            mover->Z = atof(pval.c_str());
            break;
        case hash("time"):
            mover->Duration = atof(pval.c_str());
            break;
        case hash("wait"):
            mover->Wait = atof(pval.c_str());
            break;
        case hash("ease"):
            mover->Function = easings[pval];
            break;
        }
    }
    action(sprite, *mover, 0);
    movers.push_back(make_tuple(sprite, mover, action));
}

bool SpriteManager::CheckPattern(std::string move)
{
    using namespace boost::algorithm;
    smatch match;
    move.erase(remove(move.begin(), move.end(), ' '), move.end());
    bool m = regex_match(move, match, srmove);
    return m;
}

void SpriteManager::Tick(double delta)
{
    auto i = movers.begin();
    while (i != movers.end())
    {
        auto t = *i;
        auto mover = get<1>(t);
        if (mover->Wait > 0)
        {
            //—]‚Á‚½delta‚ÅŒÄ‚Ño‚·‚×‚«‚È‚Ì‚©‚à‚µ‚ê‚È‚¢‚¯‚Ç‚Ü‚ ‚¢‚¢‚©‚Á‚Ä
            mover->Wait -= delta;
            ++i;
            continue;
        }
        bool result = get<2>(t)(get<0>(t), *mover, delta);
        mover->Now += delta;
        if (mover->Now >= mover->Duration || result)
        {
            get<2>(t)(get<0>(t), *mover, -1);
            delete mover;
            i = movers.erase(i);
        }
        else
        {
            ++i;
        }
    }
}

bool SpriteManager::ActionMoveTo(std::shared_ptr<Sprite> target, Mover &mover, double delta)
{
    if (delta == 0)
    {
        mover.Extra1 = target->Transform.X;
        mover.Extra2 = target->Transform.Y;
        return false;
    }
    else if (delta >= 0)
    {
        target->Transform.X = mover.Function(mover.Now, mover.Duration, mover.Extra1, mover.X - mover.Extra1);
        target->Transform.Y = mover.Function(mover.Now, mover.Duration, mover.Extra2, mover.Y - mover.Extra2);
        return false;
    }
    else
    {
        target->Transform.X = mover.X;
        target->Transform.Y = mover.Y;
        return true;
    }
}

bool SpriteManager::ActionMoveBy(std::shared_ptr<Sprite> target, Mover &mover, double delta)
{
    if (delta == 0)
    {
        mover.Extra1 = target->Transform.X;
        mover.Extra2 = target->Transform.Y;
        return false;
    }
    else if (delta >= 0)
    {
        target->Transform.X = mover.Function(mover.Now, mover.Duration, mover.Extra1, mover.X);
        target->Transform.Y = mover.Function(mover.Now, mover.Duration, mover.Extra2, mover.Y);
        return false;
    }
    else
    {
        target->Transform.X = mover.Extra1 + mover.X;
        target->Transform.Y = mover.Extra2 + mover.Y;
        return true;
    }
}
