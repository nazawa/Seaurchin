/*
SceneManager
ƒQ[ƒ€’†Scene‚ğˆê‹C‚É‘€ì‚·‚é‚â‚Â

*/

#include "SceneManager.h"
#include "Debug.h"

using namespace std;

SceneManager::SceneManager(shared_ptr<AngelScript> script)
{
    ScriptInterface = script;
    
}

SceneManager::~SceneManager()
{
    
}

void SceneManager::Tick(double delta)
{
    sort(Scenes.begin(), Scenes.end(), [](shared_ptr<Scene> sa, shared_ptr<Scene> sb) { return sa->GetIndex() < sb->GetIndex(); });
    auto i = Scenes.begin();
    while (i != Scenes.end())
    {
        (*i)->Tick(delta);
        if ((*i)->IsDead())
        {
            i = Scenes.erase(i);
        }
        else
        {
            i++;
        }
    }
}

void SceneManager::Draw()
{
    for (const auto& s : Scenes) s->Draw();
}

void SceneManager::AddScene(std::shared_ptr<Scene> scene)
{
    Scenes.push_back(scene);
}
