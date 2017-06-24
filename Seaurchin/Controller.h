#pragma once

enum ControllerSource {
    RawKeyboard,
    RawTouch,
    IntegratedSliders,
};

struct ControllerFingerState {
    int Id;
    WacomMTFingerState State;
    int SliderPosition;
};

class ControlState final {
    friend int WacomFingerCallback(WacomMTFingerCollection *fingerPacket, void *userData);

private:
    char KeyboardCurrent[256];
    char KeyboardLast[256];
    char KeyboardTrigger[256];
    char IntegratedCurrent[16];
    char IntegratedLast[16];
    char IntegratedTrigger[16];
    char SliderKeyboardNumbers[16];

    bool IsWacomDeviceAvailable = false;
    int *WacomDeviceIds = nullptr;
    WacomMTCapability *WacomDeviceCapabilities = nullptr;
    std::unordered_map<int, std::shared_ptr<ControllerFingerState>> CurrentFingers;
    std::mutex FingerMutex;
    void InitializeWacomTouchDevice();
    void UpdateWacomTouchDeviceFinger(WacomMTFingerCollection *fingers);

public:
    void Initialize();
    void Terminate();
    void Update();

    bool GetTriggerState(ControllerSource source, int number);
    bool GetCurrentState(ControllerSource source, int number);
    void SetSliderKey(int sliderNumber, int keyboardNumber);
};