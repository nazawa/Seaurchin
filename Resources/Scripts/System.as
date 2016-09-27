[EntryPoint]
class SystemMenu : CoroutineScene {
  Sprite sprite;
  Shape shape;
  Image@ bg;
  void Initialize() {
    //bg = LoadSystemImage("Background.png");
    @bg = LoadSystemImage("Background.png");
    sprite.SetImage(@bg);
    AddMove(sprite, "move_to(x:100, y:100, time:2, ease:out_elastic)");
    shape.Width = 100;
    shape.Height = 100;
    shape.Transform.X = 640;
    shape.Transform.Y = 320;
  }
  
  void Run() {
    WriteDebugConsole("Yo!\n");
    YieldTime(4.0);
  }
  
  void Draw() {
    sprite.Draw();
    shape.Draw();
  }
}