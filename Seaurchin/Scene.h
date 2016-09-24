#pragma once
#include "Sprite.h"
#include "SpriteManager.h"

typedef struct
{
    char Current[256];
    char Last[256];
    char Trigger[256];
} KeyState;

typedef struct
{
    std::shared_ptr<KeyState> Key;
} SharedInfo;

class Scene
{
protected:
    int index;
    std::shared_ptr<SharedInfo> sharedInfo;
    SpriteManager manager;

public:
    Scene();
    ~Scene();

    inline void SetIndex(int i) { index = i; }
    inline int GetIndex() { return index; }
    inline void SetSharedInfo(std::shared_ptr<SharedInfo> info) { sharedInfo = info; }
    inline std::shared_ptr<SharedInfo> GetSharedInfo() { return sharedInfo; }

    virtual void Initialize();
    virtual void Tick(double delta);
    virtual void Draw();
    virtual bool IsDead();
    virtual void AddMove(std::shared_ptr<Sprite> sprite, const std::string &move);
};