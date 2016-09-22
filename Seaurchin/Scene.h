#pragma once

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
};