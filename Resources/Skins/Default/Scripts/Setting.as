[EntryPoint]
class SettingScene : CoroutineScene {
  Font@ font32, font64;
  Image@ imgWhite, imgCursor;
  
  array<string> groups = {
    "一般",
    "グラフィック",
    "音声",
    "プレイ"
  };
  array<array<SettingItem@>> settings = {
    { 
      
    },
    {
      GetSettingItem("Graphic", "WaitVSync")
    },
    {
      GetSettingItem("Sound", "BufferLatency"),
      GetSettingItem("Sound", "VolumeTap"),
      GetSettingItem("Sound", "VolumeExTap"),
      GetSettingItem("Sound", "VolumeFlick"),
      GetSettingItem("Sound", "VolumeAir"),
      GetSettingItem("Sound", "VolumeAirAction")
    },
    {
      GetSettingItem("Play", "EnableAutoPlay"),
      GetSettingItem("Play", "SeenDuration")
    }
  };
  
  Sprite@ spBack, spCursor;
  array<TextSprite@> spDescriptions(10);
  array<TextSprite@> spValues(10);
  TextSprite@ spGroup;
  
  array<SettingItem@>@ target;
  int availableCount = 0;
  bool isSelectingGroup = true;
  int selected = 0;
  int selGroup = 0;
  
  void Initialize() {
    LoadResources();
    
    @spBack = Sprite(imgWhite);
    spBack.Apply("z:-10");
    AddSprite(spBack);
    
    @spGroup = TextSprite(font64, "");
    spGroup.Apply("r:0, g:0, b:0, x:640, y:16, z:1");
    spGroup.SetAlignment(TextAlign::Center, TextAlign::Top);
    AddSprite(spGroup);
    
    @spCursor = Sprite(imgCursor);
    spCursor.Apply("x:640, y:108, origX:512, origY:20, z:-1");
    AddSprite(spCursor);
    
    for(int i = 0; i < 10; i++) {
      int y = 108 + 4 + 48 * i;
      @spDescriptions[i] = TextSprite(font32, "");
      spDescriptions[i].Apply("r:0, g:0, b:0, x:384, y:" + y);
      spDescriptions[i].SetAlignment(TextAlign::Center, TextAlign::Center);
      @spValues[i] = TextSprite(font32, "");
      spValues[i].Apply("r:0, g:0, b:0, x:896, y:" + y);
      spValues[i].SetAlignment(TextAlign::Center, TextAlign::Center);
      AddSprite(spDescriptions[i]);
      AddSprite(spValues[i]);
    }
  }
  
  void LoadResources() {
    Skin@ skin = GetSkin();
    @font32 = skin.GetFont("Normal32");
    @font64 = skin.GetFont("Normal64");
    @imgWhite = skin.GetImage("White");
    @imgCursor = skin.GetImage("CursorSetting");
  }
  
  void Draw() { }
  void Run() {
    UpdateCursorState();
    LoadSettingGroup(selGroup);
    RunCoroutine(Coroutine(KeyInput), "Setting:KeyInput");
    while(true) YieldTime(1);
  }
  
  void LoadSettingGroup(int index) {
    @target = @settings[index];
    availableCount = target.length();
    spGroup.SetText("← " + groups[index] + " →");
    for(int i = 0; i < 10; i++) {
      spDescriptions[i].SetText((i < availableCount) ? target[i].GetDescription() : "");
      spValues[i].SetText((i < availableCount) ? target[i].GetItemText() : "");
    }
    if (availableCount == 0) spDescriptions[0].SetText("設定項目なし");
  }
  
  void UpdateCursorState() {
    if (isSelectingGroup) {
      spCursor.AddMove("alpha(x:1, y:0, time:0.2)");
      spGroup.Apply("b:255");
      spGroup.SetText("← " + groups[selGroup] + " →");
    } else {
      spCursor.AddMove("alpha(x:0, y:1, time:0.2)");
      spGroup.Apply("b:0");
      spGroup.SetText(groups[selGroup]);
    }
  }
  
  void KeyInput() {
    while(true) {
      if (IsKeyTriggered(Key::INPUT_UP)) {
        if (!isSelectingGroup && selected <= 0) {
          selected = 0;
          isSelectingGroup = true;
          UpdateCursorState();
        } else {
          selected--;
          int cy = 108 + 48 * selected;
          spCursor.AddMove("move_to(x:640, y:" + cy + ", time:0.2, ease:out_sine)");
        }
      }
      if (IsKeyTriggered(Key::INPUT_DOWN)) {
        if (isSelectingGroup && availableCount > 0) {
          selected = 0;
          isSelectingGroup = false;
          UpdateCursorState();
        } else {
          if (++selected >= availableCount) selected--;
          int cy = 108 + 48 * selected;
          spCursor.AddMove("move_to(x:640, y:" + cy + ", time:0.2, ease:out_sine)");
        }
      }
      if (IsKeyTriggered(Key::INPUT_LEFT)) {
        if (isSelectingGroup) {
          if (--selGroup < 0) selGroup++;
          LoadSettingGroup(selGroup);
        } else {
          target[selected].MovePrevious();
          spValues[selected].SetText(target[selected].GetItemText());
        }
      }
      if (IsKeyTriggered(Key::INPUT_RIGHT)) {
        if (isSelectingGroup) {
          if (++selGroup >= groups.length()) selGroup--;
          LoadSettingGroup(selGroup);
        } else {
          target[selected].MoveNext();
          spValues[selected].SetText(target[selected].GetItemText());
        }
      }
      if (IsKeyTriggered(Key::INPUT_ESCAPE)) {
        if (Execute("Title.as")) Disappear();
      }
      YieldFrame(1);
    }
  }
}