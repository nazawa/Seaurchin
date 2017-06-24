#pragma once


class ExecutionManager;
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
    virtual void OnEvent(const std::string &message);
    virtual void Draw();
    virtual bool IsDead();
};