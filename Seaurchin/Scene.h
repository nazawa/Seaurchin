#pragma once


class ExecutionManager;

typedef struct
{
    char Current[256];
    char Last[256];
    char Trigger[256];
} KeyState;

class Scene
{
protected:
    int index;
    ExecutionManager *manager;

public:
    Scene();
    virtual ~Scene();

    inline void SetIndex(int i) { index = i; }
    inline int GetIndex() { return index; }
    inline void SetManager(ExecutionManager *info) { manager = info; }
    inline ExecutionManager* GetManager() { return manager; }

    virtual void Initialize();
    virtual void Tick(double delta);
    virtual void Draw();
    virtual bool IsDead();
};