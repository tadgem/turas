#include "test.h"
#include "Debug/FontBinaries/icons_font_awesome.h"
#include "Debug/FontBinaries/icons_kenney.h"

class ImGuiTest : public turas::System
{
public:
  ImGuiTest() : System(turas::GetTypeHash<ImGuiTest>())
  {
  }

  void OnEngineReady() override
  {
  }

  void OnSceneLoaded(turas::Scene* scene) override
  {
  }

  void OnSceneClosed(turas::Scene* scene) override
  {
  }

  void OnUpdate(turas::Scene* scene) override
  {
    if (ImGui::Begin("Test Some Buttons"))
    {
      if(ImGui::Button("I am a button btw!"))
      {

      }

      if(ImGui::Button(ICON_FA_PLAY))
      {

      }

      if(ImGui::Button(ICON_KI_DPAD))
      {

      }
    }
    ImGui::End();
  }

  void OnShutdown()
  {
  }

  void SerializeSceneBinary(turas::Scene* scene,
                            turas::BinaryOutputArchive& output) const override
  {
  }

  void DeserializeSceneBinary(turas::Scene* scene,
                              turas::BinaryInputArchive& input) override
  {
  }

  turas::Vector<turas::AssetHandle>
  GetRequiredAssets(turas::Scene* scene) override { return {}; }
};

int main()
{
  turas::Engine e;
  e.AddSystem<ImGuiTest>();
  e.CreateScene("Testing ImGui");
  e.Init();
  e.Run();
  e.Shutdown();
  return 0;
}
