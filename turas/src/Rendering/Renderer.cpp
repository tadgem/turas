#include "Rendering/Renderer.h"


turas::Renderer::Renderer() {

}

void turas::Renderer::Start() {
    m_VK.Start("Turas", 1280, 720);
}

void turas::Renderer::Shutdown() {
    m_VK.Cleanup();
}

void turas::Renderer::PreFrame() {
    m_VK.PreFrame();
}

void turas::Renderer::PostFrame() {
    m_VK.PostFrame();
}
