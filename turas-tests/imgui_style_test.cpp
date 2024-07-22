#include "test.h"

class ImGuiTest : public turas::System {
public:
  void OnEngineReady() override {}
  void OnSceneLoaded(turas::Scene *scene) override {}
  void OnSceneClosed(turas::Scene *scene) override {}
  void OnUpdate(turas::Scene *scene) override {
    if(ImGui::Begin("Test Some Buttons"))
    {

    }
    ImGui::End();
  }
  void OnShutdown() {}
  void SerializeSceneBinary(turas::Scene *scene,
                            turas::BinaryOutputArchive &output) const override {
  }
  void DeserializeSceneBinary(turas::Scene *scene,
                              turas::BinaryInputArchive &input) override {}
  turas::Vector<turas::AssetHandle>
  GetRequiredAssets(turas::Scene *scene) override {}
};

int main() {
  turas::Engine e;
  e.Init();
  e.Run();
  e.Shutdown();
  return 0;
}