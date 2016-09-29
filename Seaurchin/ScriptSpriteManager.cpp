#include "ScriptSpriteManager.h"

using namespace std;
using namespace boost::xpressive;

unordered_map<string, function<bool(SSprite*, Mover&, double)>> ScriptSpriteMover::actions =
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


}

void ScriptSpriteMover::AddMove(std::string move)
{
    Mover *mover = new Mover{ 0 };
    auto action = actions[SpriteManager::ParseMover(mover, move)];
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
