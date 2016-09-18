#pragma once

class Scene
{
private:
    int index;

public:
    Scene();
    ~Scene();

    inline void SetIndex(int i) { index = i; }
    inline int GetIndex() { return index; }

    virtual void Tick(double delta);
    virtual void Draw();
    virtual bool IsDead();
};