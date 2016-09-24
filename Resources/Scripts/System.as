[EntryPoint]
class SystemMenu : CoroutineScene {
  Image imgbg;
  ImageSprite background;
  
  Font sysfont;
  string[] mtxt = {
    "フォントデータの作成",
    "調整",
    "終了"
  };
  TextSprite[] tsprs;
  TextSprite tscursor;
  
  int rows = 0;
  int cursor = 0;
  int height = 720;
  
  void Initialize() {
    sysfont = LoadSystemFont("System32");
    imgbg = LoadSystemImage("Background.png");
    background = CreateImageSprite();
    background.Source = imgbg;
    
    rows = mtxt.length();
    for(uint i = 0; i < rows; i++) {
      tsprs.insertLast(CreateTextSprite());
      tsprs[i].Font = sysfont;
      tsprs[i].SetText(mtxt[i]);
      tsprs[i].Transform.X = 64;
      tsprs[i].Transform.Y = i * 32;
    }
    tscursor = CreateTextSprite();
    tscursor.Font = sysfont;
    tscursor.SetText(">>");
  }
  
  void Run() {
    int c = 1;
    AddMove(background, "move_to(x:256, y:256, time:4.0, ease:out_elastic)");
    while(true) {
      if (IsKeyTriggered(Key::INPUT_DOWN)) {
        cursor = ++cursor % rows;
        tscursor.Transform.Y = cursor * 32;
      }
      if (IsKeyTriggered(Key::INPUT_UP)) {
        cursor = (cursor + rows - 1) % rows;
        tscursor.Transform.Y = cursor * 32;
      }
      YieldFrame(1);
    }
  }
  
  void Draw() {
    background.Draw();
    for(uint i = 0; i < tsprs.length(); i++) tsprs[i].Draw();
    tscursor.Draw();
  }
}