#include "Game/GameStateSleepManagement.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

void GameStateSleepManagement::OnEnter() noexcept {
    /* DO NOTHING */
}

void GameStateSleepManagement::OnExit() noexcept {
    /* DO NOTHING */
}

void GameStateSleepManagement::BeginFrame() noexcept {
    /* DO NOTHING */
}

void GameStateSleepManagement::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    
}

void GameStateSleepManagement::Render() const noexcept {
    g_theRenderer->ResetModelViewProjection();
    g_theRenderer->SetRenderTargetsToBackBuffer();
    g_theRenderer->ClearDepthStencilBuffer();

    g_theRenderer->ClearColor(Rgba::Black);

    g_theRenderer->SetViewportAsPercent();

    //2D View / HUD
    const auto& ui_view_height = currentGraphicsOptions.WindowHeight;
    const auto ui_view_width = ui_view_height * _ui_camera.GetAspectRatio();
    const auto ui_view_extents = Vector2{ui_view_width, ui_view_height};
    const auto ui_view_half_extents = ui_view_extents * 0.5f;
    auto ui_leftBottom = Vector2{-ui_view_half_extents.x, ui_view_half_extents.y};
    auto ui_rightTop = Vector2{ui_view_half_extents.x, -ui_view_half_extents.y};
    auto ui_nearFar = Vector2{0.0f, 1.0f};
    _ui_camera.position = ui_view_half_extents;
    _ui_camera.SetupView(ui_leftBottom, ui_rightTop, ui_nearFar, MathUtils::M_16_BY_9_RATIO);
    g_theRenderer->SetCamera(_ui_camera);

}

void GameStateSleepManagement::EndFrame() noexcept {
    /* DO NOTHING */
}

void GameStateSleepManagement::ShowDebugWindow() {
    if(ImGui::Begin("Debug Window", &_show_debug_window)) {
    }
    ImGui::End();
}

void GameStateSleepManagement::ToggleShowDebugWindow() noexcept {
    _show_debug_window = !_show_debug_window;
}

