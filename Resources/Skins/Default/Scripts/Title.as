[EntryPoint]
class Title : CoroutineScene {
  ImageSprite isp;
  void Initialize() {
    auto skin = GetSkinObject();
    isp = CreateImageSprite();
    isp.Source = skin.GetImage("LogoDxLib");
    AddMove(isp, "alpha(x:0, y:1, time:0.5)");
    AddMove(isp, "alpha(x:1, y:0, time:0.5, wait:2.5)");
  }
  
  void Run() {
    RunCoroutine(Coroutine(co_sample));
    while(true) YieldTime(1);
  }
  
  void Draw() {
    isp.Draw();
  }
  
  void co_sample() {
    while(true) {
      WriteDebugConsole("Yo!\n");
      YieldTime(0.5);
    }
  }
}