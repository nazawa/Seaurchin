#pragma once

#include <Windows.h>
#include <DxLib.h>
#include <angelscript.h>
#include <chrono>
#include <string>
#include <sstream>
#include <ios>

#include "Config.h"
#include "Scene.h"

class SceneDebug final : public Scene
{
private:
    int call;
    double calc;
    double fps;

public:
    ~SceneDebug();

    void Tick(double delta) override;
    void Draw() override;
    bool IsDead() override;
};