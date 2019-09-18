#include "Game/Game.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/KerningFont.hpp"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector4.hpp"

#include "Engine/Renderer/AnimatedSprite.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Window.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

void Game::Initialize() {
    g_theRenderer->RegisterMaterialsFromFolder(std::string{ "Data/Materials" });
    g_thePhysicsSystem->AddObject(_test1);
    g_thePhysicsSystem->AddObject(_test2);
    g_thePhysicsSystem->DebugShowCollision(true);
}

void Game::BeginFrame() {
    /* DO NOTHING */
}

void Game::Update(TimeUtils::FPSeconds deltaSeconds) {
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::Esc)) {
        g_theApp->SetIsQuitting(true);
        return;
    }
    g_theRenderer->UpdateGameTime(deltaSeconds);
    if(_show_debug_window) {
        ShowDebugWindow();
    }

    Camera2D& base_camera = _ui_camera;
    HandleDebugInput(deltaSeconds, base_camera);
    HandlePlayerInput(deltaSeconds, base_camera);
    base_camera.Update(deltaSeconds);

}

void Game::Render() const {
    g_theRenderer->ResetModelViewProjection();
    g_theRenderer->SetRenderTargetsToBackBuffer();
    g_theRenderer->ClearDepthStencilBuffer();

    g_theRenderer->ClearColor(Rgba::Black);

    g_theRenderer->SetViewportAsPercent();

    //2D View / HUD
    const float ui_view_height = GRAPHICS_OPTION_WINDOW_HEIGHT;
    const float ui_view_width = ui_view_height * _ui_camera.GetAspectRatio();
    const auto ui_view_extents = Vector2{ ui_view_width, ui_view_height };
    const auto ui_view_half_extents = ui_view_extents * 0.5f;
    auto ui_leftBottom = Vector2{ -ui_view_half_extents.x, ui_view_half_extents.y };
    auto ui_rightTop = Vector2{ ui_view_half_extents.x, -ui_view_half_extents.y };
    auto ui_nearFar = Vector2{ 0.0f, 1.0f };
    _ui_camera.position = ui_view_half_extents;
    _ui_camera.SetupView(ui_leftBottom, ui_rightTop, ui_nearFar, MathUtils::M_16_BY_9_RATIO);
    g_theRenderer->SetCamera(_ui_camera);

    g_theRenderer->DrawAxes(static_cast<float>((std::max)(ui_view_extents.x, ui_view_extents.y)), false);

    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__2D"));
    
}

void Game::EndFrame() {
    /* DO NOTHING */
}

void Game::HandlePlayerInput(TimeUtils::FPSeconds /*deltaSeconds*/, Camera2D& /*base_camera*/) {
    if(g_theInputSystem->IsKeyDown(KeyCode::Enter)) {
        const auto window_pos = g_theInputSystem->GetCursorWindowPosition(*g_theRenderer->GetOutput()->GetWindow());
        const auto displacement = _test1.transform.GetTranslation().GetXY() - window_pos;
        const auto dir = displacement.GetNormalize();
        const auto magnitude = 1000.0f;
        _test1.ApplyImpulse(dir * magnitude);
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::L)) {
        const auto window_pos = g_theInputSystem->GetCursorWindowPosition(*g_theRenderer->GetOutput()->GetWindow());
        const auto displacement = _test1.transform.GetTranslation().GetXY() - window_pos;
        const auto dir = displacement.GetNormalize();
        const auto magnitude = 1000.0f;
        _test1.ApplyImpulseAt(window_pos, dir * magnitude);
    }
}

void Game::ShowDebugWindow() {
    if(!ImGui::Begin("Debug Window", &_show_debug_window, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::End();
    }
    ImGui::End();
}

void Game::HandleDebugInput(TimeUtils::FPSeconds deltaSeconds, Camera2D& base_camera) {
    HandleDebugKeyboardInput(deltaSeconds, base_camera);
    HandleDebugMouseInput(deltaSeconds, base_camera);
}

void Game::HandleDebugKeyboardInput(TimeUtils::FPSeconds deltaSeconds, Camera2D& base_camera) {
    if(g_theUISystem->GetIO().WantCaptureKeyboard) {
        return;
    }
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::F1)) {
        _show_debug_window = !_show_debug_window;
    }
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::F4)) {
        g_theUISystem->ToggleImguiDemoWindow();
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::I)) {
        base_camera.Translate(-Vector2::Y_AXIS * 10.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::J)) {
        base_camera.Translate(-Vector2::X_AXIS * 10.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::K)) {
        base_camera.Translate(Vector2::Y_AXIS * 10.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::L)) {
        base_camera.Translate(Vector2::X_AXIS * 10.0f * deltaSeconds.count());
    }
}

void Game::HandleDebugMouseInput(TimeUtils::FPSeconds /*deltaSeconds*/, Camera2D& /*base_camera*/) {
    if(g_theUISystem->GetIO().WantCaptureMouse) {
        return;
    }
}
