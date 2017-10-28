#pragma once

#define SU_NOTE_LONG_MASK  0b00000000000111000000
#define SU_NOTE_SHORT_MASK 0b00000000000000111110

enum class SusNoteType : uint16_t {
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

    Injection = 17,
    Invisible,
    Unused3,
};

struct SusRelativeNoteTime {
    uint32_t Measure;
    uint32_t Tick;

    bool operator<(const SusRelativeNoteTime& b) const
    {
        return Measure < b.Measure || Tick < b.Tick;
    }
    bool operator>(const SusRelativeNoteTime& b) const
    {
        return Measure > b.Measure || Tick > b.Tick;
    }
    bool operator==(const SusRelativeNoteTime& b) const
    {
        return Measure == b.Measure && Tick == b.Tick;
    }
    bool operator!=(const SusRelativeNoteTime& b) const
    {
        return Measure != b.Measure || Tick != b.Tick;
    }
};


struct SusHispeedData {
    enum class Visibility {
        Keep = -1,
        Invisible,
        Visible,
    };
    const static double KeepSpeed;

    Visibility VisibilityState = Visibility::Visible;
    double Speed = 1.0;

};

class SusHispeedTimeline final {
private:
    std::vector<std::pair<SusRelativeNoteTime, SusHispeedData>> keys;
    std::vector<std::tuple<double, double, SusHispeedData>> Data;
    std::function<double(uint32_t, uint32_t)> RelToAbs;

public:
    SusHispeedTimeline(std::function<double(uint32_t, uint32_t)> func);
    void AddKeysByString(const std::string &def, std::function<std::shared_ptr<SusHispeedTimeline>(uint32_t)> resolver);
    void AddKeyByData(uint32_t meas, uint32_t tick, double hs);
    void AddKeyByData(uint32_t meas, uint32_t tick, bool vis);
    void Finialize();
    std::tuple<bool, double> GetRawDrawStateAt(double time);
};

enum class SusMetaDataFlags : uint16_t {
    DisableMetronome
};

struct SusMetaData {
    std::string UTitle;
    std::string USubTitle;
    std::string UArtist;
    std::string UJacketFileName;
    std::string UDesigner;
    std::string USongId;
    std::string UWaveFileName;
    double WaveOffset = 0;
    double BaseBpm = 0;
    uint32_t Level;
    uint32_t DifficultyType;
    std::string UExtraDifficulty = "";
    std::bitset<8> ExtraFlags;
};


struct SusRawNoteData {
    std::bitset<20> Type;
    std::shared_ptr<SusHispeedTimeline> Timeline;
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
    std::shared_ptr<SusHispeedTimeline> Timeline;
    uint8_t StartLane;
    uint8_t Length;

    //実描画位置
    double ModifiedPosition;
    double StartTimeEx;
    double DurationEx;
    //描画"始める"時刻
    double StartTime;
    //描画が"続く"時刻
    double Duration;
    //スライド・AA用制御データ
    std::vector<std::shared_ptr<SusDrawableNoteData>> ExtraData;

    std::tuple<bool, double> GetStateAt(double time);
};


//BMS派生フォーマットことSUS(SeaUrchinScore)の解析
class SusAnalyzer final {
private:
    static boost::xpressive::sregex RegexSusCommand;
    static boost::xpressive::sregex RegexSusData;

    double DefaultBeats = 4.0;
    double DefaultBpm = 120.0;
    uint32_t DefaultHispeedNumber = std::numeric_limits<uint32_t>::max();
    uint32_t TicksPerBeat;
    double LongInjectionPerBeat;
    std::function<std::shared_ptr<SusHispeedTimeline>(uint32_t)> TimelineResolver = nullptr;
    std::function<void(uint32_t, std::string, std::string)> ErrorCallback = nullptr;
    std::vector<std::tuple<SusRelativeNoteTime, SusRawNoteData>> Notes;
    std::vector<std::tuple<SusRelativeNoteTime, SusRawNoteData>> BpmChanges;
    std::unordered_map<uint32_t, double> BpmDefinitions;
	std::unordered_map<uint32_t, float> BeatsDefinitions;
    std::unordered_map<uint32_t, std::shared_ptr<SusHispeedTimeline>> HispeedDefinitions;
    std::shared_ptr<SusHispeedTimeline> HispeedToApply;

    void ProcessCommand(const boost::xpressive::smatch &result, bool onlyMeta);
    void ProcessRequest(const std::string &cmd);
    void ProcessData(const boost::xpressive::smatch &result);

public:
    SusMetaData SharedMetaData;
    std::vector<std::tuple<double, double>> SharedBpmChanges;

    SusAnalyzer(uint32_t tpb);
    ~SusAnalyzer();

    void Reset();
    void LoadFromFile(const std::wstring &fileName, bool analyzeOnlyMetaData = false);
    void RenderScoreData(std::vector<std::shared_ptr<SusDrawableNoteData>> &data);
    float GetBeatsAt(uint32_t measure);
    double GetBpmAt(uint32_t measure, uint32_t tick);
    double GetAbsoluteTime(uint32_t measure, uint32_t tick);
    std::tuple<uint32_t, uint32_t> GetRelativeTime(double time);
    uint32_t GetRelativeTicks(uint32_t measure, uint32_t tick);
};