[EntryPoint]
class Play : CoroutineScene {
  Skin@ skin;
  ScenePlayer@ player;
  Font@ font32, font64, fontLatin;
  Image@ p_lg, p_tap, p_extap, p_flick;
  Image@ imgWhite;
  
  void Initialize() {
    LoadResources();
    
    @player = CreatePlayer();
    player.Initialize();
    player.SetResource("LaneGround", p_lg);
    player.SetResource("Tap", p_tap);
    player.SetResource("ExTap", p_extap);
    player.SetResource("Flick", p_flick);
    
    AddSprite(Sprite(imgWhite));
  }
  
  void Run() {
    
    RunCoroutine(Coroutine(Main), "Play:Main");
    YieldTime(1);
    player.Play();
    //RunCoroutine(Coroutine(KeyInput), "Select:KeyInput");
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
    
    @p_tap = skin.GetImage("*Note-Tap");
    @p_extap = skin.GetImage("*Note-ExTap");
    @p_flick = skin.GetImage("*Note-Flick");
    @p_lg = skin.GetImage("*Lane-Ground");
    @imgWhite = skin.GetImage("White");
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
  }
}