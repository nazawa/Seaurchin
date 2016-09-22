[EntryPoint]
class SystemMenu : CoroutineScene {
  Image imgbg;
  ImageSprite background;
  Font sysfont;
  TextSprite text;
  int cursor = 0;
  int height = 720;
  
  void Initialize() {
    sysfont = LoadSystemFont("System32");
    imgbg = LoadSystemImage("Background.png");
    background = CreateImageSprite();
    background.Source = imgbg;
    text = CreateTextSprite();
    text.Font = sysfont;
    text.SetText("汚いなさすが忍者きたない");
  }
  
  void Run() {
    int c = 1;
    double time = 0;
    while(true) {
      if (IsKeyTriggered(Key::INPUT_DOWN)) cursor++;
      if (IsKeyTriggered(Key::INPUT_UP)) cursor--;
      YieldFrame(1);
    }
  }
  
  void Draw() {
    background.Draw();
    for(int i = 0; i < 1024; i++) {
      //text.Transform.X = i % 8 * 160;
      //text.Transform.Y = 32 * (i / 8);
      text.Draw();
    }
  }
}