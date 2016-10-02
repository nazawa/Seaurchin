[EntryPoint]
class SystemMenu : CoroutineScene {
  string font = "mgenplus-2pp-regular.ttf";
  void Initialize() {
    
  }
  
  void Run() {
    while(true) {
      if (IsKeyTriggered(Key::INPUT_0)) {
        CreateImageFont("C:\\Windows\\Fonts\\" + font, "System32", 32);
      }
      if (IsKeyTriggered(Key::INPUT_1)) {
        CreateImageFont("C:\\Windows\\Fonts\\" + font, "System64", 64);
      }
      if (IsKeyTriggered(Key::INPUT_2)) {
        CreateImageFont("C:\\Windows\\Fonts\\" + font, "System128", 128);
      }
      YieldFrame(1);
    }
  }
  
  void Draw() {
    
  }
}