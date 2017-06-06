[EntryPoint]
class Title : CoroutineScene {
  Skin@ skin;
  MusicCursor@ cursor;
  Font@ font32, font64, fontLatin;
  Image@ imgWhite, imgBarMusic;
  
  void Initialize() {
    LoadResources();
    AddSprite(Sprite(imgWhite));
  }
  
  void Run() {
    ReloadMusic();
    @cursor = CreateMusicCursor();
    
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
  }
  
  //ここからコルーチン
  array<TextSprite@> titles(19);
  void Main() {
    for(int i = 0; i < 19; i++) {
      double x = sin(3.1415926 * (1.0 / 18.0) * i) * 64.0;
      @titles[i] = TextSprite(font32, "");
      titles[i].Apply("y:" + (i * 40 - 16));
      titles[i].Apply("x:" + (1048 - x));
      titles[i].Apply("origX:20, r:0, g:0, b:0");
      AddSprite(titles[i]);
      
      Sprite@ cover = Sprite(imgBarMusic);
      cover.Apply("y:" + i * 40);
      cover.Apply("x:" + (1000 - x));
      cover.Apply("origY:20, z:1");
      AddSprite(cover);
    }
    UpdateCursor();
    while(true) YieldFrame(1);
  }
  
  void UpdateCursor() {
    for(int i = 0; i < 19; i++) titles[i].SetText(cursor.GetPrimaryString(i - 8));
  }
  
  void KeyInput() {
    while(true) {
      if (IsKeyTriggered(Key::INPUT_RETURN)) {
        if (cursor.Enter() == 2) {
          Disappear();
          Execute("Play.as");
        }
        UpdateCursor();
      } else if (IsKeyTriggered(Key::INPUT_ESCAPE)) {
        cursor.Exit();
        UpdateCursor();
      } else if (IsKeyTriggered(Key::INPUT_DOWN)) { 
        cursor.Next();
        UpdateCursor();
      } else if (IsKeyTriggered(Key::INPUT_UP)) {
        cursor.Previous();
        UpdateCursor();
      }
      YieldFrame(1);
    }
  }
}