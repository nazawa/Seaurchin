[EntryPoint]
void InitializeSkin(Skin@ skin) {
  //システム(というか組み込みのプレイ画面)用リソース名には先頭に*をつける
  skin.LoadImage("*Note-Tap", "Note-Tap.png");
  skin.LoadImage("*Note-ExTap", "Note-ExTap.png");
  skin.LoadImage("*Note-Air", "Note-Air.png");
  skin.LoadImage("*Note-Flick", "Note-Flick.png");
  skin.LoadImage("*Lane-Ground", "Lane-Ground.png");
  
  skin.LoadImage("White", "White.png");               //つなぎ
  skin.LoadImage("TitleBack", "Background.png");          //1280x720
  skin.LoadImage("LogoSeaurchin", "SeaurchinLogo.png");   //320x320
  skin.LoadImage("LogoDxLib", "DxLogo.jpg");              //204^2
  skin.LoadImage("LogoAngelScript", "aslogo.png");        //311x135
  skin.LoadImage("LogoBoost", "boost.png");               //277x86
  skin.LoadImage("LogoFreeType", "fond2-small.png");      //584x388
  
  skin.LoadImage("CursorMenu", "MenuCursor.png");         //320x64
  skin.LoadImage("CursorCategory", "MusicSelect-Category.png");   //480x40
  skin.LoadImage("CursorMusic", "MusicSelect-Music.png");         //480x40
  
  skin.LoadFont("Normal32", "Normal32.sif");
  skin.LoadFont("Normal64", "Normal64.sif");
  skin.LoadFont("Latin128", "Latin128.sif");
  
  skin.LoadSound("SoundCursor", "button01a.mp3");
  
}