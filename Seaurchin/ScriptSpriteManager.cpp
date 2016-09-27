#include "ScriptSpriteManager.h"

using namespace std;
using namespace boost::xpressive;

unordered_map<string, function<bool(SSprite*, Mover&, double)>> ScriptSpriteManager::actions =
{
    { "move_to", ScriptSpriteManager::ActionMoveTo },
    { "move_by", ScriptSpriteManager::ActionMoveBy },
    { "alpha", ScriptSpriteManager::ActionAlpha },
};

ScriptSpriteManager::ScriptSpriteManager()
{
}

void ScriptSpriteManager::AddMove(SSprite* sprite, std::string move)
{
    Mover *mover = new Mover{ 0 };
    auto action = actions[SpriteManager::ParseMover(mover, move)];
    //sprite->AddRef();
    action(sprite, *mover, 0);
    movers.push_back(make_tuple(sprite, mover, action));
}

void ScriptSpriteManager::Tick(double delta)
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
            get<0>(t)->Release();
            delete mover;
            i = movers.erase(i);
        }
        else
        {
            ++i;
        }
    }
}

bool ScriptSpriteManager::ActionMoveTo(SSprite* target, Mover &mover, double delta)
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

bool ScriptSpriteManager::ActionMoveBy(SSprite* target, Mover &mover, double delta)
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

bool ScriptSpriteManager::ActionAlpha(SSprite* target, Mover &mover, double delta)
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