#pragma once

#define SU_NOTE_LONG_MASK  0b00000000000111000000
#define SU_NOTE_SHORT_MASK 0b00000000000000111110

enum SusNoteType : uint16_t {
	Undefined = 0,  // BPMノーツなど
    Tap,            // Invisible Stepに使う
    ExTap,          // 中割り コンボだけ加算
	Flick,
	Air,
    HellTap,

    Hold = 6,
    Slide,
	AirAction,

    Start = 9,
    Step,
	Control,
    End,

    Up = 13,
    Down,
    Left,
    Right,

    Unused1 = 17,
    Unused2,
    Unused3,
};

struct SusRelativeNoteTime {
    uint32_t Measure;
    uint32_t Tick;
};

struct SusRawNoteData {
    std::bitset<20> Type;
    union {
        uint16_t DefinitionNumber;
        struct {
            uint8_t StartLane;
            uint8_t Length;
        } NotePosition;
    };
    uint8_t Extra;
};

struct SusDrawableNoteData {
    std::bitset<20> Type;
    std::bitset<8> OnTheFlyData;
    uint8_t StartLane;
    uint8_t Length;

    //描画"始める"時刻
    double StartTime;
    //描画が"続く"時刻
    double Duration;
    //スライド・AA用制御データ
    std::vector<std::shared_ptr<SusDrawableNoteData>> ExtraData;
};

struct SusMetaData {
    std::string UTitle;
    std::string USubTitle;
    std::string UArtist;
    std::string UJacketFileName;
    std::string UDesigner;
    std::string USongId;
    std::string UWaveFileName;
    double WaveOffset;
    uint32_t Level;
    uint32_t DifficultyType;
};

//BMS派生フォーマットことSUS(SeaUrchinScore)の解析
class SusAnalyzer final {
private:
    static boost::xpressive::sregex RegexSusCommand;
    static boost::xpressive::sregex RegexSusData;

    double DefaultBeats = 4.0;
    double DefaultBpm = 120.0;
    uint32_t TicksPerBeat;
    double LongInjectionPerBeat;
    std::function<void(uint32_t, std::string, std::string)> ErrorCallback = nullptr;
    std::vector<std::tuple<SusRelativeNoteTime, SusRawNoteData>> Notes;
    std::vector<std::tuple<SusRelativeNoteTime, SusRawNoteData>> BpmChanges;
    std::unordered_map<uint32_t, double> BpmDefinitions;
	std::unordered_map<uint32_t, float> BeatsDefinitions;

    void ProcessCommand(const boost::xpressive::smatch &result);
    void ProcessData(const boost::xpressive::smatch &result);

public:
    SusMetaData SharedMetaData;

    SusAnalyzer(uint32_t tpb);
    ~SusAnalyzer();

    void Reset();
    void LoadFromFile(const std::string &fileName, bool analyzeOnlyMetaData = false);
    void RenderScoreData(std::vector<std::shared_ptr<SusDrawableNoteData>> &data);
    float GetBeatsAt(uint32_t measure);
    double GetBpmAt(uint32_t measure, uint32_t tick);
    double GetAbsoluteTime(uint32_t measure, uint32_t tick);
    std::tuple<uint32_t, uint32_t> GetRelativeTime(double time);
    uint32_t GetRelativeTicks(uint32_t measure, uint32_t tick);
};