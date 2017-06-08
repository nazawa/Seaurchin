[EntryPoint]
class Play : CoroutineScene {
  Skin@ skin;
  Image@ imgWhite;
  Font@ font32, font64, fontLatin;
  
  ScenePlayer@ player;
  
  void Initialize() {
    @player = ScenePlayer();
    player.Initialize();
    LoadResources();
    AddSprite(Sprite(imgWhite));
  }
  
  void Run() {
    
    RunCoroutine(Coroutine(Main), "Play:Main");
    RunCoroutine(Coroutine(KeyInput), "Play:KeyInput");
    player.Play();
    YieldTime(1);
    while(true) YieldTime(30);
  }
  
  void Draw() {
    player.DrawLanes();
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
    player.SetResource("Flick", skin.GetImage("*Note-Flick"));
    player.SetResource("Hold", skin.GetImage("*Note-Hold"));
    player.SetResource("HoldStrut", skin.GetImage("*Note-HoldStrut"));
    player.SetResource("Slide", skin.GetImage("*Note-Slide"));
    player.SetResource("SlideStrut", skin.GetImage("*Note-SlideStrut"));
    player.SetResource("SoundTap", skin.GetSound("*Sound-Tap"));
    player.SetResource("SoundExTap", skin.GetSound("*Sound-ExTap"));
    player.SetResource("SoundFlick", skin.GetSound("*Sound-Flick"));
  }
  
  TextSprite @time;
  void Main() {
    @time = TextSprite(font64, "0.00");
    AddSprite(time);
    
    while(true) {
      time.SetText(formatFloat(player.GetSeenObjectsCount(), '', 3, 4));
      YieldTime(0.01);
    }
  }
  
  void KeyInput() {
    while(true) {
      if (IsKeyTriggered(Key::INPUT_ESCAPE)) {
        Disappear();
        Execute("Select.as");
      }
      YieldFrame(1);
    }
  }
}