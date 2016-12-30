#include "ScriptSpriteManager.h"

using namespace std;
using namespace boost::xpressive;
auto& srset = boost::xpressive::set;

sregex ScriptSpriteMover::srparam = (s1 = +_w) >> ':' >> (s2 = +_w | (!(srset = '+', '-') >> +_d >> !('.' >> +_d)));
sregex ScriptSpriteMover::srmove = bos >> (s1 = +_w) >> !('(' >> (s2 = ScriptSpriteMover::srparam >> *(',' >> ScriptSpriteMover::srparam)) >> ')') >> eos;

unordered_map<string, Easing::EasingFunction> ScriptSpriteMover::easings =
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
    { "in_sine", Easing::InSine },
    { "out_sine", Easing::OutSine },
    { "inout_sine", Easing::InOutSine },
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

unordered_map<string, MoverFunction> ScriptSpriteMover::actions =
{
    { "move_to", ScriptSpriteMover::ActionMoveTo },
    { "move_by", ScriptSpriteMover::ActionMoveBy },
    { "angle_to", ScriptSpriteMover::ActionAngleTo },
    { "angle_by", ScriptSpriteMover::ActionAngleBy },
    { "scale_to", ScriptSpriteMover::ActionScaleTo },
    { "alpha", ScriptSpriteMover::ActionAlpha },
    { "death", ScriptSpriteMover::ActionDeath },
};

ScriptSpriteMover::ScriptSpriteMover(SSprite *target)
{
    Target = target;
}

ScriptSpriteMover::~ScriptSpriteMover()
{
    for (auto& i : movers) delete get<0>(i);
    movers.clear();
}

bool ScriptSpriteMover::CheckPattern(std::string move)
{
    using namespace boost::algorithm;
    smatch match;
    move.erase(remove(move.begin(), move.end(), ' '), move.end());
    bool m = regex_match(move, match, srmove);
    return m;
}

string ScriptSpriteMover::ParseMover(Mover * mover, std::string move)
{
    using namespace boost::algorithm;
    constexpr auto hash = &crc_ccitt::checksum;
    smatch match;
    move.erase(remove(move.begin(), move.end(), ' '), move.end());

    mover->Function = Easing::Linear;

    bool m = regex_match(move, match, srmove);
    if (!m) return "";
    auto ms = match[s1].str();
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

    return ms;
}

tuple<string, vector<tuple<string, string>>> ScriptSpriteMover::ParseRaw(const string & move)
{
    using namespace boost::algorithm;
    constexpr auto hash = &crc_ccitt::checksum;
    smatch match;
    auto fmtm = move;
    fmtm.erase(remove(fmtm.begin(), fmtm.end(), ' '), fmtm.end());
    bool m = regex_match(fmtm, match, srmove);
    if (!m) return make_tuple("", vector<tuple<string, string>>());

    vector<tuple<string, string>> retp;
    vector<string> params;
    auto ms = match[s1].str();
    split(params, match[s2].str(), is_any_of(","));
    for (const auto& s : params)
    {
        regex_match(s, match, srparam);
        auto pname = match[s1].str();
        auto pval = match[s2].str();
        retp.push_back(make_tuple(pname, pval));
    }
    return make_tuple(ms, retp);
}


void ScriptSpriteMover::AddMove(std::string move)
{
    Mover *mover = new Mover{ 0 };
    auto aname = ParseMover(mover, move);
    auto action = actions[aname];
    if (!action)
    {
        auto rd = ParseRaw(move);
        action = Target->GetCustomAction(get<0>(rd));
        if (!action) return;
        Target->ParseCustomMover(mover, get<1>(rd));
    }
    movers.push_back(make_tuple(mover, action));
}

void ScriptSpriteMover::Tick(double delta)
{
    auto i = movers.begin();
    while (i != movers.end())
    {
        auto t = *i;
        auto mover = get<0>(t);
        if (mover->Wait > 0)
        {
            //余ったdeltaで呼び出すべきなのかもしれないけどまあいいかって
            mover->Wait -= delta;
            ++i;
            continue;
        }
        else if (mover->Wait != -1e+10)
        {
            //多分初期化処理はこのタイミングの方がいい
            mover->Wait = -1e+10;
            get<1>(t)(Target, *mover, 0);
        }
        bool result = get<1>(t)(Target, *mover, delta);
        mover->Now += delta;
        if (mover->Now >= mover->Duration || result)
        {
            get<1>(t)(Target, *mover, -1);
            delete mover;
            i = movers.erase(i);
        }
        else
        {
            ++i;
        }
    }
}

bool ScriptSpriteMover::ActionMoveTo(SSprite* target, Mover &mover, double delta)
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

bool ScriptSpriteMover::ActionMoveBy(SSprite* target, Mover &mover, double delta)
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

bool ScriptSpriteMover::ActionAngleTo(SSprite * target, Mover & mover, double delta)
{
    if (delta == 0)
    {
        mover.Extra1 = target->Transform.Angle;
        return false;
    }
    else if (delta >= 0)
    {
        target->Transform.Angle = mover.Function(mover.Now, mover.Duration, mover.Extra1, mover.X - mover.Extra1);
        return false;
    }
    else
    {
        target->Transform.Angle = mover.X;
        return true;
    }
}

bool ScriptSpriteMover::ActionAngleBy(SSprite * target, Mover & mover, double delta)
{
    if (delta == 0)
    {
        mover.Extra1 = target->Transform.Angle;
        return false;
    }
    else if (delta >= 0)
    {
        target->Transform.Angle = mover.Function(mover.Now, mover.Duration, mover.Extra1, mover.X);
        return false;
    }
    else
    {
        target->Transform.Angle = mover.Extra1 + mover.X;
        return true;
    }
}

bool ScriptSpriteMover::ActionScaleTo(SSprite *target, Mover &mover, double delta)
{
    if (delta == 0)
    {
        mover.Extra1 = target->Transform.ScaleX;
        mover.Extra2 = target->Transform.ScaleY;
        return false;
    }
    else if (delta >= 0)
    {
        target->Transform.ScaleX = mover.Function(mover.Now, mover.Duration, mover.Extra1, mover.X - mover.Extra1);
        target->Transform.ScaleY = mover.Function(mover.Now, mover.Duration, mover.Extra2, mover.Y - mover.Extra2);
        return false;
    }
    else
    {
        target->Transform.ScaleX = mover.X;
        target->Transform.ScaleY = mover.Y;
        return true;
    }
}

bool ScriptSpriteMover::ActionAlpha(SSprite* target, Mover &mover, double delta)
{
    if (delta == 0)
    {
        target->Color.A = (uint8_t)(mover.X * 255.0);
        return false;
    }
    else if (delta >= 0)
    {
        target->Color.A = (uint8_t)(255.0 * mover.Function(mover.Now, mover.Duration, mover.X, mover.Y - mover.X));
        return false;
    }
    else
    {
        target->Color.A = (uint8_t)(mover.Y * 255.0);
        return true;
    }
}

bool ScriptSpriteMover::ActionDeath(SSprite * target, Mover & mover, double delta)
{
    if (delta >= 0)
    {
        return false;
    }
    else
    {
        target->Dismiss();
        return true;
    }
}
