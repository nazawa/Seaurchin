#pragma once

enum ControllerSource {
    RawKeyboard,
    RawTouch,
    IntegratedSliders,
};

class ControlState final {
private:
    char KeyboardCurrent[256];
    char KeyboardLast[256];
    char KeyboardTrigger[256];

    char IntegratedCurrent[16];
    char IntegratedLast[16];
    char IntegratedTrigger[16];

    char SliderKeyboardNumbers[16];

public:
    void Initialize();
    void Terminate();
    void Update();

    bool GetTriggerState(ControllerSource source, int number);
    bool GetCurrentState(ControllerSource source, int number);
    void SetSliderKey(int sliderNumber, int keyboardNumber);
};