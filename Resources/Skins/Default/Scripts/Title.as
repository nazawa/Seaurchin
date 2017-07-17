[EntryPoint]
class Title : CoroutineScene {
  Skin@ skin;
  Font@ fontLatin, font32, font64;
  Image@ imgWhite, imgDxLib, imgBoost, imgFreeType, imgAngelScript, imgSeaurchin;
  Image@ imgCursorMenu;
  
  void Initialize() {
    LoadResources();
    AddSprite(Sprite(imgWhite));
  }
  
  void Run() {
    if (!ExistsData("LogoShown")) {
      SetData("LogoShown", true);
      RunCoroutine(Coroutine(Intro), "Title:Intro");
      YieldTime(4.0);
    }
    RunCoroutine(Coroutine(TitleRipple), "Title:Ripple");
    RunCoroutine(Coroutine(KeyInput), "Title:KeyInput");
    while(true) YieldTime(30);
  }
  
  void Draw() {
    
  }
  
  void LoadResources() {
    @skin = GetSkin();
    @fontLatin = skin.GetFont("Latin128");
    @font32 = skin.GetFont("Normal32");
    @font64 = skin.GetFont("Normal64");
    
    @imgWhite = skin.GetImage("White");
    @imgDxLib = skin.GetImage("LogoDxLib");
    @imgBoost = skin.GetImage("LogoBoost");
    @imgFreeType = skin.GetImage("LogoFreeType");
    @imgAngelScript = skin.GetImage("LogoAngelScript");
    @imgSeaurchin = skin.GetImage("LogoSeaurchin");
    @imgCursorMenu = skin.GetImage("CursorMenu");
    @smCursor = skin.GetSound("SoundCursor");
    @mixSE = GetDefaultMixer("SE");
  }
  
  //ここからコルーチン
  void Intro() {
    
    array<Sprite@> dxl = { 
      TextSprite(font64, "Powered by"),
      TextSprite(fontLatin, "DxLib"),
      Sprite(imgDxLib)
    };
    dxl[0].Apply("x:416, y:92, r:0, g:0, b: 0");
    dxl[1].Apply("x:416, y:162, r:0, g:0, b: 0");
    dxl[2].Apply("x:200, y:200, origY:102");
    dxl[2].HasAlpha = false;
    for(int i = 0; i < dxl.length(); i++) {
      dxl[i].AddMove("alpha(x:0, y:1, time:1)");
      dxl[i].AddMove("alpha(x:1, y:0, time:1, wait:3)");
      dxl[i].AddMove("death(wait:4)");
      AddSprite(dxl[i]);
    }
    
    array<Sprite@> logo = {
      Sprite(imgBoost),
      Sprite(imgFreeType),
      Sprite(imgAngelScript)
    };
    logo[0].Apply("x:200, y:300");
    logo[1].Apply("x:516, y:300");
    logo[2].Apply("x:206, y:500");
    for(int i = 0; i < logo.length(); i++) {
      logo[i].AddMove("alpha(x:0, y:1, time:1)");
      logo[i].AddMove("alpha(x:1, y:0, time:1, wait:3)");
      logo[i].AddMove("death(wait:4)");
      AddSprite(logo[i]);
    }
  }
  
  Sprite@ spLogo, spCursor;
  ClipSprite@ spTitle;
  array<Sprite@> menu(3);
  Sound@ smCursor;
  SoundMixer@ mixSE;
  int mcur = 0;
  void TitleRipple() {
    @spLogo = Sprite(imgSeaurchin);
    spLogo.Apply("origX:160, origY:160, x:640, y:360, z:2, alpha:0");
    spLogo.AddMove("alpha(y:1, time:0.5)");
    AddSprite(spLogo);
    
    @spTitle = ClipSprite(640, 128);
    TextSprite @buffer = TextSprite(font64, "Ground Slider Simulator");
    buffer.Apply("r:0, g:0, b:0");
    spTitle.Transfer(buffer);
    buffer.SetText("Seaurchin");
    buffer.Apply("y:64");
    spTitle.Transfer(buffer);
    spTitle.Apply("origY:64, x:640, y:360, z:1");
    spTitle.SetRange(0, 0, 0, 1);
    AddSprite(spTitle);
    
    @menu[0] = TextSprite(font64, "Start");
    @menu[1] = TextSprite(font64, "Setting");
    @menu[2] = TextSprite(font64, "Exit");
    
    @spCursor = Sprite(imgCursorMenu);
    spCursor.Apply("x:-320, y:" + (400 + 64 * mcur));
    AddSprite(spCursor);
    
    for(int i = 0; i < 3; i++) {
      menu[i].Apply(dictionary = {
        {"x", 0/*-320*/}, {"y", 400 + 64 * i},
        {"origX", 160},
        {"r", 0}, {"g", 0}, {"b", 0}
      });
      AddSprite(menu[i]);
    }
    
    while(true) {
      YieldTime(2);
      auto ripple = spLogo.Clone();
      ripple.Apply("alpha:0.5");
      ripple.AddMove("alpha(x:0.3, y:0, time:1.0)");
      ripple.AddMove("scale_to(x:1.5, y:1.5, time:1.0)");
      ripple.AddMove("death(wait:1.0)");
      AddSprite(ripple);
    }
  }
  
  void KeyInput() {
    while(true) {
      if (IsKeyTriggered(Key::INPUT_RETURN)) {
        spLogo.AddMove("move_by(x:-260, time:0.5, ease:out_sine)");
        spLogo.AddMove("scale_to(x:0.4, y:0.4, time:0.5, ease:out_sine)");
        spLogo.AddMove("move_by(y:-240, time:0.5, wait:0.5, ease:out_sine)");
        spTitle.AddMove("move_by(x:-200, time:0.5, ease:out_sine)");
        spTitle.AddMove("range_size(width:1.0, height:1.0, time:0.5, ease:out_sine)");
        spTitle.AddMove("move_by(y:-240, time:0.5, wait:0.5, ease:out_sine)");
        YieldTime(1.0);
        break;
      }
      YieldFrame(1);
    }
    for(int i = 0; i < 3; i++) menu[i].AddMove("move_by(x:720, time:0.25, ease:out_sine)");
    spCursor.AddMove("move_by(x:800, time:0.25, ease:out_sine)");
    while(true) {
      if (IsKeyTriggered(Key::INPUT_UP)) {
        mcur = (mcur + 2) % 3;
        mixSE.Play(smCursor);
        spCursor.AddMove("move_to(time:0.1, x:480, y:" + (400 + 64 * mcur) + ")");
      }
      if (IsKeyTriggered(Key::INPUT_DOWN)) {
        mcur = (mcur + 1) % 3;
        mixSE.Play(smCursor);
        spCursor.AddMove("move_to(time:0.1, x:480, y:" + (400 + 64 * mcur) + ")");
      }
      if (IsKeyTriggered(Key::INPUT_RETURN)) {
        switch(mcur) {
          case 0:
            if (Execute("Select.as")) Disappear();
            break;
          case 1:
            if (Execute("Setting.as")) Disappear();
            break;
          case 2:
            ExitApplication();
            break;
        }
        
      }
      YieldFrame(1);
    }
  }
}