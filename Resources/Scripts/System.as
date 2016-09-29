[EntryPoint]
class SystemMenu : CoroutineScene {
  Image@ titleback, bg;
  Sprite@ tbb, back;
  void Initialize() {
    @bg = LoadSystemImage("Background.png");
    @back = Sprite(bg);
    AddSprite(back);
    
    @titleback = LoadSystemImage("test1.png");
    @tbb = Sprite(titleback);
    tbb.Apply("origX:64, origY:128, scaleX:0.7, scaleY:0.7, alpha:0, angle:0.3");
    for(int i = 0; i < 21; i++) {
      auto cs = tbb.Clone();
      cs.Transform.X = 1280 / 24 * i;
      cs.Transform.Y = 360;
      //入り
      cs.AddMove("scale_to(x:1, y:1, time:0.3, ease:out_back, wait:" + 0.05 * i + ")");
      cs.AddMove("alpha(x:0, y:1, time:0.3, wait:" + 0.05 * i + ")");
      cs.AddMove("angle_to(x:0, time:0.2, ease:out_quad, wait:" + 0.05 * i + ")");
      cs.AddMove("move_to(x:" + 1280 / 20 * i + ", y:360, time:0.3, ease:out_quad, wait:" + 0.05 * i + ")");
      
      //抜き
      cs.AddMove("scale_to(x:0.7, y:0.7, time:0.3, ease:in_back, wait:" + (0.05 * i + 3) + ")");
      cs.AddMove("alpha(x:1, y:0, time:0.3, wait:" + (0.05 * i + 3) + ")");
      cs.AddMove("angle_to(x:0.2, time:0.2, ease:out_quad, wait:" + (0.05 * i + 3) + ")");
      cs.AddMove("move_to(x:" + 1280 / 17 * i + ", y:360, time:0.3, ease:out_quad, wait:" + (0.05 * i + 3) + ")");
      AddSprite(cs);
    }
  }
  
  void Run() {
    WriteDebugConsole("Yo!\n");
    while(true) YieldTime(1);
  }
  
  void Draw() {
    
  }
}