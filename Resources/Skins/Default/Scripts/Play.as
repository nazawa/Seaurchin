[EntryPoint]
class Play : CoroutineScene {
  Skin@ skin;
  Image@ imgWhite;
  Font@ font32, font64, fontLatin;
  ScenePlayer@ player;
  
  void Initialize() {
    @player = ScenePlayer();
    LoadResources();
    player.Initialize();  //呼ぶ前にリソースを設定すること
  }
  
  void Run() {
    RunCoroutine(Coroutine(Main), "Play:Main");
    RunCoroutine(Coroutine(KeyInput), "Play:KeyInput");
    player.Z = 5;
    AddSprite(player);
    YieldTime(1);
    player.Play();
    while(true) YieldTime(30);
  }
  
  void Draw() {
    
  }
  
  void LoadResources() {
    @skin = GetSkin();
    @fontLatin = skin.GetFont("Latin128");
    @font32 = skin.GetFont("Normal32");
    @font64 = skin.GetFont("Normal64");
    @imgWhite = skin.GetImage("TitleBack");
    
    player.SetResource("LaneGround", skin.GetImage("*Lane-Ground"));
    player.SetResource("FontCombo", font64);
    player.SetResource("Tap", skin.GetImage("*Note-Tap"));
    player.SetResource("ExTap", skin.GetImage("*Note-ExTap"));
    player.SetResource("AirUp", skin.GetImage("*Note-AirUp"));
    player.SetResource("AirDown", skin.GetImage("*Note-AirDown"));
    player.SetResource("Flick", skin.GetImage("*Note-Flick"));
    player.SetResource("Hold", skin.GetImage("*Note-Hold"));
    player.SetResource("HoldStrut", skin.GetImage("*Note-HoldStrut"));
    player.SetResource("Slide", skin.GetImage("*Note-Slide"));
    player.SetResource("SlideStrut", skin.GetImage("*Note-SlideStrut"));
    player.SetResource("AirAction", skin.GetImage("*Note-AirAction"));
    player.SetResource("SoundTap", skin.GetSound("*Sound-Tap"));
    player.SetResource("SoundExTap", skin.GetSound("*Sound-ExTap"));
    player.SetResource("SoundFlick", skin.GetSound("*Sound-Flick"));
  }
  
  
  Sprite@ spTopCover, spBack;
  void Main() {
    @spBack = Sprite(skin.GetImage("TitleBack"));
    @spTopCover = Sprite(skin.GetImage("PlayerTopCover"));
    spTopCover.Apply("z:0");
    spTopCover.Apply("z:10");
    AddSprite(spBack);
    AddSprite(spTopCover);
    
    while(true) {
      YieldTime(0.01);
    }
  }
  
  void KeyInput() {
    while(true) {
      if (IsKeyTriggered(Key::INPUT_ESCAPE)) {
        Disappear();
        Execute("Select.as");
      }
      
      if (IsKeyTriggered(Key::INPUT_UP)) player.AdjustCamera(10, 0, 0);
      if (IsKeyTriggered(Key::INPUT_DOWN)) player.AdjustCamera(-10, 0, 0);
      if (IsKeyTriggered(Key::INPUT_ADD)) player.AdjustCamera(0, 10, 0);
      if (IsKeyTriggered(Key::INPUT_SUBTRACT)) player.AdjustCamera(0, -10, 0);
      if (IsKeyTriggered(Key::INPUT_NUMPAD8)) player.AdjustCamera(0, 0, 10);
      if (IsKeyTriggered(Key::INPUT_NUMPAD2)) player.AdjustCamera(0, 0, -10);
      
      YieldFrame(1);
    }
  }
}