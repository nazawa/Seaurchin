#include "SusAnalyzer.h"
#include "Misc.h"

using namespace std;
namespace b = boost;
namespace ba = boost::algorithm;
namespace fsys = boost::filesystem;
namespace xp = boost::xpressive;
static constexpr auto hashstr = &crc_ccitt::checksum;

xp::sregex SusAnalyzer::RegexSusCommand = "#" >> (xp::s1 = +xp::alnum) >> +xp::space >> (xp::s2 = +(~xp::_n));
xp::sregex SusAnalyzer::RegexSusData = "#" >> (xp::s1 = xp::repeat<3, 3>(xp::alnum)) >> (xp::s2 = xp::repeat<2, 3>(xp::alnum)) >> ":" >> *xp::space >> (xp::s3 = +(xp::alnum | xp::space | "."));

static xp::sregex AllNumeric = xp::bos >> +(xp::digit) >> xp::eos;

static auto ConvertInteger = [](const string &input) -> int32_t {
    return atoi(input.c_str());
};

static auto ConvertHexatridecimal = [](const string &input) -> uint32_t {
    return stoul(input, 0, 36);
};

static auto ConvertFloat = [](const string &input) -> double {
    return ToDouble(input.c_str());
};

static auto ConvertRawString = [](const string &input) -> string {
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
    } else {
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
    BeatsDefinitions.clear();
    // TicksPerBeat = ;
    SharedMetaData.UTitle = u8"";
    SharedMetaData.UArtist = u8"";
    SharedMetaData.USubTitle = u8"";
    SharedMetaData.UDesigner = u8"";
    SharedMetaData.Level = 0;
    SharedMetaData.DifficultyType = 0;

    BpmDefinitions[0] = 120.0;
    BeatsDefinitions[0] = 4.0;
}

//一応UTF-8として処理することにしますがどうせ変わらないだろうなぁ
//あと列挙済みファイルを流し込む前提でエラーチェックしない
void SusAnalyzer::LoadFromFile(const string &fileName, bool analyzeOnlyMetaData)
{
    ifstream file;
    string rawline;
    xp::smatch match;
    uint32_t line = 0;

    Reset();

    file.open(fileName, ios::in);
    while (getline(file, rawline)) {
        if (!rawline.length()) continue;
        if (rawline[0] != '#') continue;
        if (xp::regex_match(rawline, match, RegexSusCommand)) {
            ProcessCommand(match);
        } else if (xp::regex_match(rawline, match, RegexSusData)) {
            if (!analyzeOnlyMetaData) ProcessData(match);
        } else {
            if (ErrorCallback) ErrorCallback(0, "Error", "SUS有効行ですが解析できませんでした。");
        }
    }
    sort(Notes.begin(), Notes.end(), [](tuple<SusRelativeNoteTime, SusRawNoteData> a, tuple<SusRelativeNoteTime, SusRawNoteData> b) {
        return get<0>(a).Tick < get<0>(b).Tick;
    });
    stable_sort(Notes.begin(), Notes.end(), [](tuple<SusRelativeNoteTime, SusRawNoteData> a, tuple<SusRelativeNoteTime, SusRawNoteData> b) {
        return get<0>(a).Measure < get<0>(b).Measure;
    });

    if (!analyzeOnlyMetaData) RenderScoreData();
    file.close();
}

void SusAnalyzer::ProcessCommand(const xp::smatch &result)
{
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


void SusAnalyzer::ProcessData(const xp::smatch &result)
{
    auto meas = result[1].str();
    auto lane = result[2].str();
    auto pattern = result[3].str();
    ba::erase_all(pattern, " ");

    if (xp::regex_match(meas, AllNumeric)) {
        // 配置データ
        auto noteCount = pattern.length() / 2;
        auto step = (uint32_t)(TicksPerBeat * GetBeatsAt(ConvertInteger(meas))) / noteCount;

        if (lane.length() == 3) {
            // ロングタイプ
            for (auto i = 0; i < noteCount; i++) {
                auto note = pattern.substr(i * 2, 2);
                SusRawNoteData noteData;
                SusRelativeNoteTime time = { ConvertInteger(meas), step * i };
                noteData.NotePosition.StartLane = ConvertHexatridecimal(lane.substr(1, 1));
                noteData.NotePosition.Length = ConvertHexatridecimal(note.substr(1, 1));
                noteData.Extra = ConvertHexatridecimal(lane.substr(2, 1));

                switch (lane[0]) {
                    case '2':
                        noteData.Type.set(SusNoteType::Hold);
                        break;
                    case '3':
                        noteData.Type.set(SusNoteType::Slide);
                        break;
                    case '4':
                        noteData.Type.set(SusNoteType::AirAction);
                        break;
                    default:
                        if (ErrorCallback) ErrorCallback(0, "Error", "ロングレーンの指定が不正です。");
                        break;
                }
                switch (note[0]) {
                    case '1':
                        noteData.Type.set(SusNoteType::Start);
                        break;
                    case '2':
                        noteData.Type.set(SusNoteType::End);
                        break;
                    case '3':
                        noteData.Type.set(SusNoteType::Step);
                        break;
                    case '4':
                        noteData.Type.set(SusNoteType::Control);
                        break;
                    default:
                        if (ErrorCallback) ErrorCallback(0, "Error", "ノーツ種類の指定が不正です。");
                        break;
                }
                Notes.push_back(make_tuple(time, noteData));
            }
        } else if (lane[0] >= '1') {
            // ショートタイプ
            for (auto i = 0; i < noteCount; i++) {
                auto note = pattern.substr(i * 2, 2);
                SusRawNoteData noteData;
                SusRelativeNoteTime time = { ConvertInteger(meas), step * i };
                noteData.NotePosition.StartLane = ConvertHexatridecimal(lane.substr(1, 1));
                noteData.NotePosition.Length = ConvertHexatridecimal(note.substr(1, 1));

                switch (note[0]) {
                    case '1':
                        noteData.Type.set(SusNoteType::Tap);
                        break;
                    case '2':
                        noteData.Type.set(SusNoteType::ExTap);
                        break;
                    case '3':
                        noteData.Type.set(SusNoteType::Flick);
                        break;
                    case '4':
                        // 本来はHell
                        noteData.Type.set(SusNoteType::Tap);
                        break;
                    case '5':
                        noteData.Type.set(SusNoteType::Air);
                        noteData.Type.set(SusNoteType::Up);
                        break;
                    case '6':
                        noteData.Type.set(SusNoteType::Air);
                        noteData.Type.set(SusNoteType::Down);
                        break;
                    case '7':
                        noteData.Type.set(SusNoteType::Air);
                        noteData.Type.set(SusNoteType::Up);
                        noteData.Type.set(SusNoteType::Left);
                        break;
                    case '8':
                        noteData.Type.set(SusNoteType::Air);
                        noteData.Type.set(SusNoteType::Up);
                        noteData.Type.set(SusNoteType::Right);
                        break;
                    case '9':
                        noteData.Type.set(SusNoteType::Air);
                        noteData.Type.set(SusNoteType::Down);
                        noteData.Type.set(SusNoteType::Left);
                        break;
                    case 'a':
                    case 'A':
                        noteData.Type.set(SusNoteType::Air);
                        noteData.Type.set(SusNoteType::Down);
                        noteData.Type.set(SusNoteType::Right);
                        break;
                    default:
                        if (ErrorCallback) ErrorCallback(0, "Error", "ショートレーンの指定が不正です。");
                        break;
                }
                Notes.push_back(make_tuple(time, noteData));
            }
        } else {
            // BPMなど
            switch (lane[1]) {
                case '2':
                    // 小節長
                    BeatsDefinitions[ConvertInteger(meas)] = ConvertFloat(pattern);
                    break;
                case '8': {
                    // BPM
                    for (auto i = 0; i < noteCount; i++) {
                        auto note = pattern.substr(i * 2, 2);
                        SusRawNoteData noteData;
                        SusRelativeNoteTime time = { ConvertInteger(meas), step * i };
                        noteData.Type.set(SusNoteType::Undefined);
                        noteData.DefinitionNumber = ConvertHexatridecimal(note);
                        Notes.push_back(make_tuple(time, noteData));
                    }
                    break;
                }
                default:
                    if (ErrorCallback) ErrorCallback(0, "Error", "不正なデータコマンドです");
                    break;
            }
        }
    } else {
        // コマンドデータ
        transform(meas.cbegin(), meas.cend(), meas.begin(), toupper);
        if (meas == "BPM") {
            auto number = ConvertHexatridecimal(lane);
            BpmDefinitions[number] = ConvertFloat(pattern);
        }
    }
}

float SusAnalyzer::GetBeatsAt(uint32_t measure)
{
    float result = 4.0;
    uint32_t last = 0;
    for (auto &t : BeatsDefinitions) {
        if (t.first >= last && t.first <= measure) {
            result = t.second;
            last = t.first;
        }
    }
    return result;
}

void SusAnalyzer::RenderScoreData()
{
    auto GetAbsoluteTime = [&](uint32_t meas, uint32_t tick) {
        double time = 0.0;
        double lastBpm = 120.0;
        vector<tuple<SusRelativeNoteTime, SusRawNoteData>> bpmchanges;

        for (int i = 0; i < meas + 1; i++) {
            auto beats = GetBeatsAt(i);
            bpmchanges.clear();
            copy_if(Notes.begin(), Notes.end(), back_inserter(bpmchanges), [i](tuple<SusRelativeNoteTime, SusRawNoteData> n) {
                return get<0>(n).Measure == i && get<1>(n).Type[SusNoteType::Undefined];
            });
            auto lastChangeTick = 0u;
            for (auto& bc : bpmchanges) {
                auto timing = get<0>(bc);
                if (i == meas && timing.Tick >= tick) break;
                double dur = (60.0 / lastBpm) * ((double)(timing.Tick - lastChangeTick) / TicksPerBeat);
                time += dur;
                lastChangeTick = timing.Tick;
                lastBpm = BpmDefinitions[get<1>(bc).DefinitionNumber];
            }
            if (i == meas) {
                time += (60.0 / lastBpm) * ((double)(tick - lastChangeTick) / TicksPerBeat);
            } else {
                time += (60.0 / lastBpm) * ((double)(TicksPerBeat * beats - lastChangeTick) / TicksPerBeat);
            }
        }

        return time;
    };

    // 不正チェックリスト
    // ショート: はみ出しは全部アウト
    // ホールド: ケツ無しアウト(ケツ連は無視)、Step/Control問答無用アウト、ケツ違いアウト
    // スライド、AA: ケツ無しアウト(ケツ連は無視)
    vector<SusDrawableNoteData> data(Notes.size());
    data.clear();
    for (auto& note : Notes) {
        auto time = get<0>(note);
        auto info = get<1>(note);
        if (info.Type[SusNoteType::Step]) continue;
        if (info.Type[SusNoteType::Control]) continue;
        if (info.Type[SusNoteType::End]) continue;
        if (info.Type[SusNoteType::Undefined]) continue;

        auto bits = info.Type.to_ulong();
        if (bits & 0b0000000000011110) {
            // ショート
            if (info.NotePosition.StartLane + info.NotePosition.Length > 16) {
                if (ErrorCallback) ErrorCallback(0, "Error", "ショートノーツがはみ出しています。");
            }
            SusDrawableNoteData noteData;
            noteData.Type = info.Type;
            noteData.StartTime = GetAbsoluteTime(time.Measure, time.Tick);
            noteData.Duration = 0;
            noteData.StartLane = info.NotePosition.StartLane;
            noteData.Length = info.NotePosition.Length;
            data.push_back(noteData);
        } else if (bits & 0b0000000011100000) {
            SusDrawableNoteData noteData;
            noteData.Type = info.Type;
            noteData.StartLane = info.NotePosition.StartLane;
            noteData.Length = info.NotePosition.Length;
        } else {
            if (ErrorCallback) ErrorCallback(0, "Error", "致命的なノーツエラー(不正な内部表現です)。");
        }
    }
    data.size();
}
