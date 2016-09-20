[EntryPoint]
class SystemMenu : CoroutineScene {
  Image imgbg;
  ImageSprite background;
  VirtualFont font;
  
  void Initialize() {
    font = CreateVirtualFont("ＭＳ ゴシック", 16);
    imgbg = LoadSystemImage("Background.png");
    background = CreateImageSprite();
    background.Source = imgbg;
  }
  
  void Run() {
    int c = 1;
    double time = 0;
    while(true) {
      YieldTime(1.0 / 60.0);
    }
  }
  
  void Draw() {
    background.Draw();
  }
}