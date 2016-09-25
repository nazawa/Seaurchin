#include "SceneDebug.h"
#include "Debug.h"

SceneDebug::~SceneDebug()
{
    WriteDebugConsole("SceneDebug Destructed.\n");
}

void SceneDebug::Tick(double delta)
{
    calc += delta;
    call++;
    if (calc >= 1.00)
    {
        fps = call / calc;
        calc = call = 0;
    }
}

void SceneDebug::Draw()
{
    clsDx();
    printfDx("%2.1f fps\n", fps);
}

bool SceneDebug::IsDead()
{
    return false;
}

