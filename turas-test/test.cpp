#include "test.h"
#include <filesystem>
BEGIN_TESTS()

TEST(
    {
        turas::log::info("Assets");
        turas::log::info("Current Path : {}", std::filesystem::current_path().string());
        turas::Engine e;
        e.m_AssetManager.LoadAsset("../Sponza/Sponza.gltf", turas::AssetType::Model);
        while(e.m_AssetManager.AnyAssetsLoading())
        {
            e.m_AssetManager.OnUpdate();
            turas::log::info("Processing Futures");

        }
        turas::log::info("End Assets");
    }
)

TEST(
    {

        turas::log::info("again!");
    }
)

RUN_TESTS()

