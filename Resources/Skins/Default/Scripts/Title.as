[EntryPoint]
class Title : CoroutineScene {
  Skin@ skin;
  Font@ fontLatin, font32, font64;
  Image@ imgWhite, imgDxLib, imgBoost, imgFreeType, imgAngelScript;
  void Initialize() {
    LoadResources();
    
    AddSprite(Sprite(imgWhite));
    
    array<Sprite> dxl = { 
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
    
    array<Sprite> logo = {
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
  
  void Run() {
    YieldTime(4.0); //ロゴ待ち
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
  }
  
  //ここからコルーチン
  Sprite @spLogo;
  ClipSprite @spTitle;
  bool isopened;
  void TitleRipple() {
    @spLogo = Sprite(imgDxLib);
    spLogo.HasAlpha = false;
    spLogo.Apply("origX:102, origY:102, x:640, y:360, z:2");
    AddSprite(spLogo);
    
    @spTitle = ClipSprite(584, 388);
    spTitle.Transfer(imgFreeType, 0, 0);
    spTitle.Apply("origY:194, x:640, y:360, z:1");
    spTitle.SetRange(0, 0, 0.01, 1);
    AddSprite(spTitle);
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
        WriteDebugConsole("Triggered\n");
        if (isopened) {
          spTitle.AddMove("move_by(x:200, time:0.5, ease:out_bounce)");
          spLogo.AddMove("move_by(x:200, time:0.5, ease:out_bounce)");
          spTitle.AddMove("range_size(width:0, height:1.0, time:0.5, ease:out_sine)");
        } else {
          spTitle.AddMove("move_by(x:-200, time:0.5, ease:out_bounce)");
          spLogo.AddMove("move_by(x:-200, time:0.5, ease:out_bounce)");
          spTitle.AddMove("range_size(width:1.0, height:1.0, time:0.5, ease:out_sine)");
        }
        isopened = isopened != true;
      }
      YieldFrame(1);
    }
  }
}