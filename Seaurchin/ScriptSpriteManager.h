#pragma once

#include "ScriptSprite.h"
#include "Easing.h"

struct Mover
{
    Easing::EasingFunction Function;
    double X;
    double Y;
    double Z;
    double Extra1;
    double Extra2;
    double Extra3;
    double Wait;
    double Now;
    double Duration;
};

using MoverFunction = std::function<bool(SSprite*, Mover&, double)>;

class ScriptSpriteMover final
{

private:
    std::list<std::tuple<Mover*, MoverFunction>> movers;
    static std::unordered_map<std::string, MoverFunction> actions;
    static std::unordered_map <std::string, Easing::EasingFunction> easings;
    static boost::xpressive::sregex srmove;
    static boost::xpressive::sregex srparam;

public:
    SSprite *Target;

    ScriptSpriteMover(SSprite *target);
    ~ScriptSpriteMover();
    
    static bool CheckPattern(std::string move);
    void AddMove(std::string move);
    static std::string ParseMover(Mover* mover, std::string move);
    static std::tuple<std::string, std::vector<std::tuple<std::string, std::string>>> ParseRaw(const std::string &move);
    void Tick(double delta);

    //delta > 0 : 動作
    //delta == 0 : Mover初期化
    //delta == -1 : 終了(位置確定などにどうぞ)
    //true返せば逸でも中段できる

    static bool ActionMoveTo(SSprite* target, Mover &mover, double delta);
    static bool ActionMoveBy(SSprite* target, Mover &mover, double delta);
    static bool ActionAngleTo(SSprite* target, Mover &mover, double delta);
    static bool ActionAngleBy(SSprite* target, Mover &mover, double delta);
    static bool ActionScaleTo(SSprite* target, Mover &mover, double delta);
    static bool ActionAlpha(SSprite* target, Mover &mover, double delta);
    static bool ActionDeath(SSprite* target, Mover &mover, double delta);
};