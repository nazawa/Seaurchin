#include "SusAnalyzer.h"
#include "Misc.h"

using namespace std;
namespace b = boost;
namespace ba = boost::algorithm;
namespace fsys = boost::filesystem;
namespace xp = boost::xpressive;
static constexpr auto hashstr = &crc_ccitt::checksum;

xp::sregex SusAnalyzer::RegexSusCommand = "#" >> (xp::s1 = +xp::alnum) >> +xp::space >> (xp::s2 = +(~xp::_n));
xp::sregex SusAnalyzer::RegexSusData = "#" >> (xp::s1 = xp::repeat<3, 3>(xp::alnum)) >> (xp::s2 = xp::repeat<2, 2>(xp::alnum)) >> ":" >> *xp::space >> (xp::s3 = +(xp::alnum | xp::space));

static auto ConvertInteger = [](string input) -> int32_t {
    return atoi(input.c_str());
};

static auto ConvertHexatridecimal = [](string input) -> uint32_t {
    return stoul(input, 0, 36);
};

static auto ConvertFloat = [](string input) -> double {
    return ToDouble(input.c_str());
};

static auto ConvertRawString = [](string input) -> string {
    // TIL: ASCII文字範囲ではUTF-8と本来のASCIIを間違うことはない
    if (ba::starts_with(input, "\"")) {
        ostringstream result;
        string rest = input;
        ba::trim_if(rest, ba::is_any_of("\""));
        auto it = rest.begin();
        while (it != rest.end()) {
            if (*it != '\\') {
                result << *it;
                it++;
                continue;
            }
            it++;
            if (it == rest.end()) return "";
            switch (*it) {
            case '"':
                result << "\"";
                break;
            case 't':
                result << "\t";
                break;
            case 'n':
                result << "\n";
                break;
            case 'u': {
                //utf-8 4byte食う
                char *cp = "0000";
                for (int i = 0; i < 4; i++) {
                    cp[i] = *(++it);
                }
                wchar_t r = stoi(cp, 0, 16);
                //でも突っ込むのめんどくさいので🙅で代用します
                result << u8"xx";
                break;
            }
            default:
                break;
            }
            it++;
        }
        return result.str();
    }
    else {
        return input;
    }
};



SusAnalyzer::SusAnalyzer(uint32_t tpb)
{
    TicksPerBeat = tpb;
}

SusAnalyzer::~SusAnalyzer()
{
    Reset();
}

void SusAnalyzer::Reset()
{
    Notes.clear();
    BpmDefinitions.clear();
    SharedMetaData.UTitle = u8"";
    SharedMetaData.UArtist = u8"";
    SharedMetaData.USubTitle = u8"";
    SharedMetaData.UDesigner = u8"";
    SharedMetaData.Level = 0;
    SharedMetaData.DifficultyType = 0;
}

//一応UTF-8として処理することにしますがどうせ変わらないだろうなぁ
//あと列挙済みファイルを流し込む前提でエラーチェックしない
void SusAnalyzer::LoadFromFile(string fileName)
{
    ifstream file;
    string rawline;
    xp::smatch match;
    uint32_t line = 0;

    file.open(fileName, ios::in);
    while (getline(file, rawline)) {
        if (!rawline.length()) continue;
        if (rawline[0] != '#') continue;
        if (xp::regex_match(rawline, match, RegexSusCommand)) {
            ProcessCommand(match);
        }
        else if (xp::regex_match(rawline, match, RegexSusData)) {
            ProcessData(match);
        }
        else {
            if (ErrorCallback) ErrorCallback(0, "Error", "SUS有効行ですが解析できませんでした。");
        }
    }
}

void SusAnalyzer::ProcessCommand(xp::smatch result) {
    auto name = result[1].str();
    transform(name.cbegin(), name.cend(), name.begin(), toupper);
    if (ba::starts_with(name, "BPM")) {
        // #BPMxx yyy.yy
        BpmDefinitions[ConvertHexatridecimal(name.substr(3))] = ConvertFloat(result[2].str());
        return;
    }
    switch (hashstr(name.c_str())) {
        //TODO:このへんはBMSに合わせる必要あり
    case hashstr("TITLE"):
        SharedMetaData.UTitle = ConvertRawString(result[2]);
        break;
    case hashstr("SUBTITLE"):
        SharedMetaData.USubTitle = ConvertRawString(result[2]);
    case hashstr("ARTIST"):
        SharedMetaData.UArtist = ConvertRawString(result[2]);
        break;
    case hashstr("GENRE"):
        //SharedMetaData.UGenre = ConvertRawString(result[2]);
        break;
    case hashstr("DESIGNER"):
    case hashstr("SUBARTIST"):  //BMS互換
        SharedMetaData.UDesigner = ConvertRawString(result[2]);
        break;
    case hashstr("PLAYLEVEL"):
        SharedMetaData.Level = ConvertInteger(result[2]);
        break;
    case hashstr("DIFFICULTY"):
        SharedMetaData.DifficultyType = ConvertInteger(result[2]);
        break;
    case hashstr("SONGID"):
        SharedMetaData.USongId = ConvertRawString(result[2]);
    }

}


void SusAnalyzer::ProcessData(xp::smatch result) {


}

void SusAnalyzer::RenderScoreData()
{
}
