#pragma once

enum SusNoteType : uint8_t {
    Undefined = 0,
    Tap,
    ExTap,
    HoldStart,
    HoldEnd,
    Air,
    Action,
    SlideStart,
    SlideChange,
    SlideControl,
    SlideEnd,
    //此処までで0x0A

    //補助属性 主にAirがからむ系に
    Up = 0b00010000,
    Down = 0b00100000,
    Left = 0b01000000,
    Right = 0b10000000,
};

struct SusNoteTime {
    uint32_t Measure;
    uint32_t Tick;
};

struct SusNoteData {
    SusNoteType Type;
    uint8_t StartLane;
    uint8_t Length;
    uint8_t Extra;
};

struct SusMetaData {
    std::string Title;
    std::string Artist;
    std::string Genre;
    std::string Designer;
    uint32_t Level;
    uint32_t DifficultyType;
};

//BMS派生フォーマットことSUS(SeaUrchinScore)の解析
class SusAnalyzer final {
private:
    static boost::xpressive::sregex RegexSusCommand;
    static boost::xpressive::sregex RegexSusData;

    uint32_t TicksPerBeat;
    std::function<void(uint32_t, std::string, std::string)> ErrorCallback = nullptr;
    std::vector<std::tuple<SusNoteTime, SusNoteData>> Notes;

    void ProcessCommand(boost::xpressive::smatch result);
    void ProcessData(boost::xpressive::smatch result);

public:
    SusAnalyzer(uint32_t tpb);
    ~SusAnalyzer();

    void Reset();
    void LoadFromFile(std::string fileName);
    void RenderScoreData();
};