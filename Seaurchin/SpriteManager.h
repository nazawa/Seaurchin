#pragma once

#include "Sprite.h"
#include "Easing.h"

class SpriteManager final
{
public:

    typedef struct
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
    } Mover;

private:
    std::list<std::tuple<std::shared_ptr<Sprite>, Mover*, std::function<bool(std::shared_ptr<Sprite>, Mover&, double)>>> movers;

    static std::unordered_map<std::string, std::function<bool(std::shared_ptr<Sprite>, Mover&, double)>> actions;
    static std::unordered_map <std::string, Easing::EasingFunction> easings;
    static boost::xpressive::sregex srmove;
    static boost::xpressive::sregex srparam;

public:
    SpriteManager();
    void AddMove(std::shared_ptr<Sprite> sprite, std::string move);
    bool CheckPattern(std::string move);

    void Tick(double delta);

    //delta > 0 : 動作
    //delta == 0 : Mover初期化
    //delta == -1 : 終了(位置確定などにどうぞ)
    //true返せば逸でも中段できる

    static bool ActionMoveTo(std::shared_ptr<Sprite> target, Mover &mover, double delta);
    static bool ActionMoveBy(std::shared_ptr<Sprite> target, Mover &mover, double delta);
    static bool ActionAlpha(std::shared_ptr<Sprite> target, Mover &mover, double delta);
};

//http://iorate.hatenablog.com/entry/20110115/1295108835
namespace crc_ccitt
{

    constexpr unsigned int process_char(unsigned int acc, int n)
    {
        return n > 0 ?
            process_char(acc & 0x8000 ? (acc << 1) ^ 0x11021 : acc << 1, n - 1) :
            acc;
    }

    constexpr unsigned int process_string(unsigned int acc, char const *s)
    {
        return *s ?
            process_string(process_char(acc ^ (*s << 8), 8), s + 1) :
            acc;
    }

    constexpr std::uint16_t checksum(char const *s)
    {
        return process_string(0xFFFF, s);
    }

    static_assert(checksum("123456789") == 0x29B1, "crc error");

}
