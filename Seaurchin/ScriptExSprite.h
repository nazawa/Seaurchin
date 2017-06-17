#pragma once

#include "ScriptResource.h"
#include "ScriptSpriteMisc.h"

class SExSprite {
protected:
    int Reference;

public:
    Transform3D Transform;
    ColorTint Color;
    bool IsDead = false;

    SExSprite();
    virtual ~SExSprite();

    void AddRef();
    void Release();
    virtual void Tick(double delta);
    virtual void Draw();
    inline void Dismiss() { IsDead = true; }
    inline void Revive() { IsDead = false; }
};

class ExSpritePrimitive {
public:
    virtual void Draw(const Transform3D &parent) = 0;
};

