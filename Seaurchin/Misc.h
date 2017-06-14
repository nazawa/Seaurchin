#pragma once

#define BEGIN_DRAW_TRANSACTION(h) SetDrawScreen(h)
#define FINISH_DRAW_TRANSACTION SetDrawScreen(DX_SCREEN_BACK);

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

// AngelScript‚É“o˜^‚µ‚½’lŒ^—p‚Ì”Ä—pˆ—ƒAƒŒ

template <typename T>
void AngelScriptValueConstruct(void *address)
{
    new (address) T;
}

template <typename T>
void AngelScriptValueDestruct(void *address)
{
    static_cast<T*>(address)->~T();
}


std::string ConvertUTF8ToShiftJis(std::string utf8str);
std::string ConvertShiftJisToUTF8(std::string sjisstr);
std::wstring ConvertUTF8ToUnicode(std::string utf8str);
void ScriptSceneWarnOutOf(std::string type, asIScriptContext *ctx);
double ToDouble(const char *str);