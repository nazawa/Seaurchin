[EntryPoint]
class Title : CoroutineScene {
  Skin@ skin;
  MusicCursor@ cursor;
  Font@ font32, font64, fontLatin;
  Image@ imgWhite, imgBarMusic, imgMusicFrame;
  
  void Initialize() {
    LoadResources();
    AddSprite(Sprite(imgWhite));
  }
  
  void Run() {
    ReloadMusic();
    @cursor = MusicCursor();
    
    RunCoroutine(Coroutine(Main), "Select:Main");
    RunCoroutine(Coroutine(KeyInput), "Select:KeyInput");
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
    @imgBarMusic = skin.GetImage("CursorMusic");
    @imgMusicFrame = skin.GetImage("MusicSelectFrame");
  }
  
  //ここからコルーチン
  array<TextSprite@> titles(19);
  array<SynthSprite@> musics(5);
  void Main() {
    for(int i = 0; i < 5; i++) {
      @musics[i] = SynthSprite(400, 600);
      musics[i].Apply("origX:200, origY:300, y: 360, z:2, x:" + (-320.0 + 480 * i));
      AddSprite(musics[i]);
    }
    InitCursor();
    while(true) YieldFrame(1);
  }
  
  Sprite@ spj;
  TextSprite@ txt;
  void InitCursor() {
    @spj = Sprite();
    @txt = TextSprite(font64, "");
    spj.Apply("scaleX:0.5, scaleY:0.5, x:40, y:40");
    txt.SetAlignment(TextAlign::Center, TextAlign::Top);
    txt.Apply("x:200, y:408, r:0, g:0, b:0");
    for(int i = 0; i < 5; i++) {
      int add = (7 - center) % 5;
      musics[i].Clear();
      musics[i].Transfer(imgMusicFrame, 0, 0);
      //ジャケ
      Image@ jacket = Image(cursor.GetMusicJacketFileName((i + add) % 5 - 2));
      spj.SetImage(jacket);
      musics[i].Transfer(spj);
      //タイトル
      txt.SetText(cursor.GetPrimaryString((i + add) % 5 - 2));
      musics[i].Transfer(txt);
    }
  }
  int center = 2;
  void UpdateCursor(int adjust) {
    for(int i = 0; i < 5; i++) {
      musics[i].AbortMove();
      int add = (7 - center) % 5;
      musics[i].Apply("x:" + (640 + 480 * ((i + add) % 5 - 2)));
    }
    int flew = (5 + center - adjust * 2) % 5;
    musics[flew].Apply("x:" + (640 + 480 * adjust * 3));
    musics[flew].Clear();
    musics[flew].Transfer(imgMusicFrame, 0, 0);
    //ジャケ
    Image@ jacket = Image(cursor.GetMusicJacketFileName(adjust * 2));
    spj.SetImage(jacket);
    musics[flew].Transfer(spj);
    //タイトル
    txt.SetText(cursor.GetPrimaryString(adjust * 2));
    musics[flew].Transfer(txt);
    center = (5 + center + adjust) % 5;
    for(int i = 0; i < 5; i++) musics[i].AddMove("move_by(x:" + (480 * -adjust) + ", time:0.2, ease:out_quad)");
  }
  
  void KeyInput() {
    while(true) {
      if (IsKeyTriggered(Key::INPUT_RETURN)) {
        if (cursor.Enter() == 2) {
          if (Execute("Play.as")) Disappear();
        } else {
          InitCursor();
        }
      } else if (IsKeyTriggered(Key::INPUT_ESCAPE)) {
        cursor.Exit();
        InitCursor();
      } else if (IsKeyTriggered(Key::INPUT_RIGHT)) { 
        cursor.Next();
        UpdateCursor(+1);
      } else if (IsKeyTriggered(Key::INPUT_LEFT)) {
        cursor.Previous();
        UpdateCursor(-1);
      }
      YieldFrame(1);
    }
  }
}