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
    while(true) YieldFrame(1);
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
}