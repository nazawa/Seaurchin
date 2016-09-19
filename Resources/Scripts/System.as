[EntryPoint]
class SystemMenu : CoroutineScene {
  Sprite sprite;
  
  void Initialize() {
    WriteDebugConsole("うんこがみっつ\n");
  }
  
  void Run() {
    int c = 1;
    while(true) {
      WriteDebugConsole("現在" + sprite + "回目\n");
      YieldTime(1.0);
    }
  }
  
  void Draw() {
    
  }
}