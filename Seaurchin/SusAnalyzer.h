#pragma once

enum SusNoteType : uint16_t {
	Undefined = 0,  // BPMノーツなど
    Tap,            // Invisible Stepに使う
    ExTap,
	Flick,
	Air,

    Hold = 5,
    Slide,
	AirAction,

    Start = 8,
    Step,
	Control,
    End,

    Up = 12,
    Down,
    Left,
    Right,
};

struct SusRelativeNoteTime {
    uint32_t Measure;
    uint32_t Tick;
};

struct SusRawNoteData {
    std::bitset<16> Type;
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
    std::bitset<16> Type;
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

    uint32_t TicksPerBeat;
    double LongInjectionPerBeat;
    std::function<void(uint32_t, std::string, std::string)> ErrorCallback = nullptr;
    std::vector<std::tuple<SusRelativeNoteTime, SusRawNoteData>> Notes;
    std::unordered_map<uint32_t, double> BpmDefinitions;
	std::unordered_map<uint32_t, float> BeatsDefinitions;

    void ProcessCommand(const boost::xpressive::smatch &result);
    void ProcessData(const boost::xpressive::smatch &result);
	float GetBeatsAt(uint32_t measure);
    double GetAbsoluteTime(uint32_t measure, uint32_t tick);
    std::tuple<uint32_t, uint32_t> GetRelativeTime(double time);

public:
    SusMetaData SharedMetaData;

    SusAnalyzer(uint32_t tpb);
    ~SusAnalyzer();

    void Reset();
    void LoadFromFile(const std::string &fileName, bool analyzeOnlyMetaData = false);
    void RenderScoreData(std::vector<std::shared_ptr<SusDrawableNoteData>> &data);
};