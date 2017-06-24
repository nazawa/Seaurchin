#include "Controller.h"

void ControlState::Initialize()
{
    ZeroMemory(KeyboardCurrent, sizeof(char) * 256);
    ZeroMemory(KeyboardLast, sizeof(char) * 256);
    ZeroMemory(KeyboardTrigger, sizeof(char) * 256);
    ZeroMemory(IntegratedCurrent, sizeof(char) * 16);
    ZeroMemory(IntegratedLast, sizeof(char) * 16);
    ZeroMemory(IntegratedTrigger, sizeof(char) * 16);

    SliderKeyboardNumbers[0] = KEY_INPUT_A;
    SliderKeyboardNumbers[1] = KEY_INPUT_Z;
    SliderKeyboardNumbers[2] = KEY_INPUT_S;
    SliderKeyboardNumbers[3] = KEY_INPUT_X;
    SliderKeyboardNumbers[4] = KEY_INPUT_D;
    SliderKeyboardNumbers[5] = KEY_INPUT_C;
    SliderKeyboardNumbers[6] = KEY_INPUT_F;
    SliderKeyboardNumbers[7] = KEY_INPUT_V;
    SliderKeyboardNumbers[8] = KEY_INPUT_G;
    SliderKeyboardNumbers[9] = KEY_INPUT_B;
    SliderKeyboardNumbers[10] = KEY_INPUT_H;
    SliderKeyboardNumbers[11] = KEY_INPUT_N;
    SliderKeyboardNumbers[12] = KEY_INPUT_J;
    SliderKeyboardNumbers[13] = KEY_INPUT_M;
    SliderKeyboardNumbers[14] = KEY_INPUT_K;
    SliderKeyboardNumbers[15] = KEY_INPUT_COMMA;


}

void ControlState::Terminate()
{}

void ControlState::Update()
{
    memcpy_s(KeyboardLast, sizeof(char) * 256, KeyboardCurrent, sizeof(char) * 256);
    GetHitKeyStateAll(KeyboardCurrent);
    for (int i = 0; i < 256; i++) KeyboardTrigger[i] = !KeyboardLast[i] && KeyboardCurrent[i];

    for (int i = 0; i < 16; i++) {
        IntegratedLast[i] = IntegratedCurrent[i];
        IntegratedCurrent[i] = KeyboardCurrent[SliderKeyboardNumbers[i]];
        IntegratedTrigger[i] = !IntegratedLast[i] && IntegratedCurrent[i];
    }
}

bool ControlState::GetTriggerState(ControllerSource source, int number)
{
    switch (source) {
        case ControllerSource::RawKeyboard:
            if (number < 0 || number >= 256) return false;
            return KeyboardTrigger[number];
        case ControllerSource::IntegratedSliders:
            if (number < 0 || number >= 16) return false;
            return IntegratedTrigger[number];
        case ControllerSource::RawTouch:
            return false;
    }
    return false;
}

bool ControlState::GetCurrentState(ControllerSource source, int number)
{
    switch (source) {
        case ControllerSource::RawKeyboard:
            if (number < 0 || number >= 256) return false;
            return KeyboardCurrent[number];
        case ControllerSource::IntegratedSliders:
            if (number < 0 || number >= 16) return false;
            return IntegratedCurrent[number];
        case ControllerSource::RawTouch:
            return false;
    }
    return false;
}

void ControlState::SetSliderKey(int sliderNumber, int keyboardNumber)
{
    if (sliderNumber < 0 || sliderNumber >= 16) return;
    if (keyboardNumber < 0 || keyboardNumber >= 256) return;
    SliderKeyboardNumbers[sliderNumber] = keyboardNumber;
}
