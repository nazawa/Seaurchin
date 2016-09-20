[EntryPoint]
class SystemMenu : CoroutineScene {
  Image imgbg;
  ImageSprite sprite;
  VirtualFont font;
  
  void Initialize() {
    font = CreateVirtualFont("Mgen+ 2m regular", 32);
    imgbg = LoadSystemImage("Background.png");
    sprite = CreateImageSprite();
    sprite.Source = imgbg;
    sprite.Transform.OriginY = 720 / 2; 
  }
  
  void Run() {
    int c = 1;
    double time = 0;
    while(true) {
      sprite.Transform.Y = 120 * sin(3.1415 * 2 * time) + 720 / 2;
      YieldTime(1.0 / 60.0);
      time += 1.0 / 60.0;
    }
  }
  
  void Draw() {
    
    for(int i = 0; i < 200; i++) sprite.Draw(); // DrawRawString(font, "The quick fox jumps over the lazy dog.", 0, i * 32);
  }
}