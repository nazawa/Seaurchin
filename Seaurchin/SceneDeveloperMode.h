#pragma once

#include "Config.h"
#include "Scene.h"

class SceneDeveloperMode final : public Scene
{
private:
    
public:
    SceneDeveloperMode();
    ~SceneDeveloperMode();

    void Initialize() override;
    void Tick(double delta) override;
    void Draw() override;
    bool IsDead() override;
};