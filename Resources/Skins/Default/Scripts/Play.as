[EntryPoint]
class Play : CoroutineScene {
  Skin@ skin;
  Image@ imgWhite;
  Image@ imgGCEmpty, imgGCFull, imgGBBack, imgGBFill, imgGBFront;
  Font@ font32, font64, fontLatin;
  ScenePlayer@ player;
  
  void Initialize() {
    LoadResources();
  }
  
  void Run() {
    RunCoroutine(Coroutine(RunPlayer), "Play:RunPlayer");
    RunCoroutine(Coroutine(Main), "Play:Main");
    RunCoroutine(Coroutine(KeyInput), "Play:KeyInput");
    YieldTime(1);
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
    @imgGCEmpty = skin.GetImage("GaugeCountEmpty");
    @imgGCFull = skin.GetImage("GaugeCountFull");
    @imgGBBack = skin.GetImage("GaugeBarBack");
    @imgGBFill = skin.GetImage("GaugeBarFill");
    @imgGBFront = skin.GetImage("GaugeBarFront");
  }
  
  void SetPlayerResource() {
    player.SetResource("LaneGround", skin.GetImage("*Lane-Ground"));
    player.SetResource("LaneHoldLight", skin.GetImage("*Lane-HoldLight"));
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
    player.SetResource("SoundAir", skin.GetSound("*Sound-Air"));
    player.SetResource("SoundAirAction", skin.GetSound("*Sound-AirAction"));
    player.SetResource("SoundHoldLoop", skin.GetSound("*Sound-SlideLoop"));
    player.SetResource("SoundSlideLoop", skin.GetSound("*Sound-HoldLoop"));
    player.SetResource("EffectTap", skin.GetAnime("*Anime-Tap"));
    player.SetResource("EffectExTap", skin.GetAnime("*Anime-ExTap"));
    player.SetResource("EffectAirAction", skin.GetAnime("*Anime-AirAction"));
    player.SetResource("EffectSlideTap", skin.GetAnime("*Anime-SlideTap"));
    player.SetResource("EffectSlideLoop", skin.GetAnime("*Anime-SlideLoop"));
  }
  
  void RunPlayer() {
    @player = ScenePlayer();
    SetPlayerResource();
    player.Initialize();
    player.Z = 5;
    AddSprite(player);
    player.Load();
    while(!player.IsLoadCompleted()) YieldFrame(1);
    player.Play();
  }
  
  Sprite@ spTopCover, spBack;
  array<Sprite@> spGaugeCounts(10);
  Sprite@ spBarBack, spBarFront;
  ClipSprite@ spBarFill;
  TextSprite@ txtScore;
  SynthSprite@ spJudges;
  void Main() {
    @spBack = Sprite(skin.GetImage("TitleBack"));
    @spTopCover = Sprite(skin.GetImage("PlayerTopCover"));
    spTopCover.Apply("z:10");
    // スコアなど
    @txtScore = TextSprite(font64, "0");
    txtScore.SetAlignment(TextAlign::Right, TextAlign::Top);
    txtScore.Apply("x:1276, y:-4, z:15");
    @spJudges = SynthSprite(384, 96);
    spJudges.Transfer(skin.GetImage("JudgeJC"), 0, 0);
    spJudges.Transfer(skin.GetImage("JudgeJ"), 0, 24);
    spJudges.Transfer(skin.GetImage("JudgeA"), 176, 0);
    spJudges.Transfer(skin.GetImage("JudgeM"), 176,24);
    spJudges.Apply("x:900, y:56, z:15");
    array<TextSprite@> spCounts = {
      TextSprite(font32, "0"),
      TextSprite(font32, "0"),
      TextSprite(font32, "0"),
      TextSprite(font32, "0")
    };
    for(int i = 0; i < 4; i++) {
      spCounts[i].Apply("x: " + (1100 + i / 2 * 176) + ", z:15, y:" + (24 * (i % 2) + 50));
      spCounts[i].SetAlignment(TextAlign::Right, TextAlign::Top);
    }
    // ゲージ
    for(int i = 0; i < 10; i++) {
      @spGaugeCounts[i] = Sprite(imgGCEmpty);
      spGaugeCounts[i].Apply("x:" + (384 + i * 48) + ", y:74, z:11, scaleX:0.8, scaleY:0.5");
      AddSprite(spGaugeCounts[i]);
    }
    @spBarBack = Sprite(imgGBBack);
    @spBarFront = Sprite(imgGBFront);
    @spBarFill = ClipSprite(512, 64);
    spBarFill.Transfer(imgGBFill, 0, 0);
    spBarFill.SetRange(0, 0, 0, 1);
    spBarBack.Apply("x:384, y:4, z:15");
    spBarFill.Apply("x:384, y:4, z:16");
    spBarFront.Apply("x:384, y:4, z:17");
    
    AddSprite(spBack);
    AddSprite(spTopCover);
    AddSprite(txtScore);
    AddSprite(spJudges);
    for(int i = 0; i < 4; i++) AddSprite(spCounts[i]);
    AddSprite(spBack);
    AddSprite(spBarBack);
    AddSprite(spBarFill);
    AddSprite(spBarFront);
    
    PlayStatus psNow, psPrev;
    int gMax = 0;
    double gCurrent = 0;
    while(true) {
      int pgm = gMax;
      double pgc = gCurrent;
      player.GetPlayStatus(psNow);
      psNow.GetGaugeValue(gMax, gCurrent);
      if (gMax > pgm) for(int i = pgm; i < gMax; i++) {
         spGaugeCounts[i].SetImage(imgGCFull);
         spGaugeCounts[i].Apply("scaleX:1.0");
         spGaugeCounts[i].AddMove("scale_to(x:0.8, y:0.5, time:0.3)");
      }
      if (gCurrent != pgc) {
        spBarFill.AddMove("range_size(width:" + formatFloat(gCurrent, '', 1, 4) + ", height:1, time:0.1, ease:out_sine)");
        txtScore.SetText("" + psNow.GetScore());
      }
      if (psNow.JusticeCritical != psPrev.JusticeCritical) {
          spCounts[0].SetText("" + psNow.JusticeCritical);
          spCounts[0].AbortMove();
          spCounts[0].Apply("scaleX:1.3");
          spCounts[0].AddMove("scale_to(x:1.0, y:1.0, time:0.3)");
        }
        if (psNow.Justice != psPrev.Justice) {
          spCounts[1].SetText("" + psNow.Justice);
          spCounts[1].AbortMove();
          spCounts[1].Apply("scaleX:1.3");
          spCounts[1].AddMove("scale_to(x:1.0, y:1.0, time:0.3)");
        }
        if (psNow.Attack != psPrev.Attack) {
          spCounts[2].SetText("" + psNow.Attack);
          spCounts[2].AbortMove();
          spCounts[2].Apply("scaleX:1.3");
          spCounts[2].AddMove("scale_to(x:1.0, y:1.0, time:0.3)");
        }
        if (psNow.Miss != psPrev.Miss) {
          spCounts[3].SetText("" + psNow.Miss);
          spCounts[3].AbortMove();
          spCounts[3].Apply("scaleX:1.3");
          spCounts[3].AddMove("scale_to(x:1.0, y:1.0, time:0.3)");
        }
      psPrev = psNow;
      YieldFrame(1);
    }
  }
  
  void KeyInput() {
    while(true) {
      if (IsKeyTriggered(Key::INPUT_ESCAPE)) {
        if (Execute("Select.as")) Disappear();
      }
      if (IsKeyTriggered(Key::INPUT_LEFT)) {
        player.MovePositionByMeasure(-1);
      }
      if (IsKeyTriggered(Key::INPUT_RIGHT)) {
        player.MovePositionByMeasure(1);
      }
      
      YieldFrame(1);
    }
  }
}