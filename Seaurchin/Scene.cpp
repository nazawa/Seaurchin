#include "Scene.h"
/*
Scene
ƒV[ƒ“‚¾‚æ

*/

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Initialize()
{
}

void Scene::Tick(double delta)
{
}

void Scene::Draw()
{
}

bool Scene::IsDead()
{
    return false;
}

void Scene::AddMove(std::shared_ptr<Sprite> sprite, const std::string & move)
{
    manager.AddMove(sprite, move);
}
