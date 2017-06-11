[EntryPoint]
void InitializeSkin(Skin@ skin) {
  //システム(というか組み込みのプレイ画面)用リソース名には先頭に*をつける
  skin.LoadImage("*Note-Tap", "Note-Tap.png");
  skin.LoadImage("*Note-ExTap", "Note-ExTap.png");
  skin.LoadImage("*Note-Air", "Note-Air.png");
  skin.LoadImage("*Note-AirUp", "Air-Up.png");
  skin.LoadImage("*Note-AirDown", "Air-Down.png");
  skin.LoadImage("*Note-Flick", "Note-Flick.png");
  skin.LoadImage("*Note-Hold", "Note-Hold.png");
  skin.LoadImage("*Note-HoldStrut", "Note-HoldStrut.png");
  skin.LoadImage("*Note-Slide", "Note-Slide.png");
  skin.LoadImage("*Note-SlideStrut", "Note-SlideStrut.png");
  skin.LoadImage("*Note-AirAction", "Note-AirAction.png");
  skin.LoadImage("*Lane-Ground", "Lane-Ground.png");
  
  skin.LoadImage("White", "White.png");               //つなぎ
  skin.LoadImage("TitleBack", "Background.png");          //1280x720
  skin.LoadImage("LogoSeaurchin", "SeaurchinLogo.png");   //320x320
  skin.LoadImage("LogoDxLib", "DxLogo.jpg");              //204^2
  skin.LoadImage("LogoAngelScript", "aslogo.png");        //311x135
  skin.LoadImage("LogoBoost", "boost.png");               //277x86
  skin.LoadImage("LogoFreeType", "fond2-small.png");      //584x388
  skin.LoadImage("PlayerTopCover", "PlayerTopCover.png"); //1280x106
  skin.LoadImage("GaugeCountEmpty", "GaugeCountEmpty.png");
  skin.LoadImage("GaugeCountFull", "GaugeCountFull.png");
  skin.LoadImage("GaugeBarBack", "GaugeBarBack.png");
  skin.LoadImage("GaugeBarFill", "GaugeBarFill.png");
  skin.LoadImage("GaugeBarFront", "GaugeBarFront.png");
  
  skin.LoadImage("MusicSelectFrame", "MusicSelect-Frame.png");
  skin.LoadImage("CursorMenu", "MenuCursor.png");         //320x64
  skin.LoadImage("CursorCategory", "MusicSelect-Category.png");   //480x40
  skin.LoadImage("CursorMusic", "MusicSelect-Music.png");         //480x40
  
  skin.LoadFont("Normal32", "Normal32.sif");
  skin.LoadFont("Normal64", "Normal64.sif");
  skin.LoadFont("Latin128", "Latin128.sif");
  
  skin.LoadSound("*Sound-Tap", "Tap.wav");
  skin.LoadSound("*Sound-ExTap", "ExTap.wav");
  skin.LoadSound("*Sound-Flick", "Flick.wav");
  skin.LoadSound("*Sound-Air", "Air.wav");
  skin.LoadSound("*Sound-AirAction", "AirAction.wav");
  skin.LoadSound("*Sound-SlideLoop", "SlideLoop.wav");
  skin.LoadSound("*Sound-HoldLoop", "SlideLoop.wav");
  skin.LoadSound("SoundCursor", "button01a.mp3");
  
}