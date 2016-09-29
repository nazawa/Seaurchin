#pragma once

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


std::string ConvertUTF8ToShiftJis(std::string utf8str);
void ScriptSceneWarnOutOf(std::string type, asIScriptContext *ctx);