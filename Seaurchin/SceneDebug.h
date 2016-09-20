#pragma once


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