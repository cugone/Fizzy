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
    _test1_OBB2.AddPaddingToSides(25.0f, 25.0f);
    _test1_OBB2.Translate(Vector2(200.0f, 250.0f));
    _test2_OBB2.AddPaddingToSides(25.0f, 25.0f);
    _test2_OBB2.Translate(Vector2(300.0f, 250.0f));
    _test1_OBB2.SetOrientationDegrees(0.0f);
    _test2_OBB2.SetOrientationDegrees(0.0f);
}

void Game::BeginFrame() {
    /* DO NOTHING */
}

void Game::Update(TimeUtils::FPSeconds deltaSeconds) {
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::Esc)) {
        g_theApp->SetIsQuitting(true);
        return;
    }

    Camera2D& base_camera = _ui_camera;
    HandleDebugInput(base_camera);
    HandlePlayerInput(base_camera);
    base_camera.Update(deltaSeconds);

    if(g_theInputSystem->IsKeyDown(KeyCode::R)) {
        _test2_OBB2.SetOrientationDegrees(0.0f);
        _test1_OBB2.SetOrientationDegrees(0.0f);
    }

    float speed = 50.0f;
    if(g_theInputSystem->IsKeyDown(KeyCode::D)) {
        _test2_OBB2.Translate(Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::A)) {
        _test2_OBB2.Translate(-Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::W)) {
        _test2_OBB2.Translate(-Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::S)) {
        _test2_OBB2.Translate(Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::Backspace)) {
        _test2_OBB2.position = Vector2(225.0f, 225.0f);
    }

    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad6)) {
        _test1_OBB2.Translate(Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad4)) {
        _test1_OBB2.Translate(-Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad8)) {
        _test1_OBB2.Translate(-Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad5)) {
        _test1_OBB2.Translate(Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::Q)) {
        _test2_OBB2.SetOrientationDegrees(_test2_OBB2.orientationDegrees - 45.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::E)) {
        _test2_OBB2.SetOrientationDegrees(_test2_OBB2.orientationDegrees + 45.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad7)) {
        _test1_OBB2.SetOrientationDegrees(_test1_OBB2.orientationDegrees - 45.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad9)) {
        _test1_OBB2.SetOrientationDegrees(_test1_OBB2.orientationDegrees + 45.0f * deltaSeconds.count());
    }

    _do_overlap = MathUtils::DoOBBsOverlap(_test1_OBB2, _test2_OBB2);
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
    _ui_camera.SetupView(ui_leftBottom, ui_rightTop, ui_nearFar, MathUtils::M_16_BY_9_RATIO);
    g_theRenderer->SetCamera(_ui_camera);

    g_theRenderer->DrawAxes(static_cast<float>(std::max(ui_view_extents.x, ui_view_extents.y)), false);

    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__2D"));
    g_theRenderer->SetModelMatrix(Matrix4::I);
    g_theRenderer->DrawOBB2(_test1_OBB2, _do_overlap ? Rgba::White : Rgba::Red, Rgba::NoAlpha);

    g_theRenderer->SetModelMatrix(Matrix4::I);
    g_theRenderer->DrawOBB2(_test2_OBB2, _do_overlap ? Rgba::White : Rgba::Green, Rgba::NoAlpha);

}

void Game::EndFrame() {
    /* DO NOTHING */
}

void Game::HandlePlayerInput(Camera2D& /*base_camera*/) {
    /* DO NOTHING */
}

void Game::HandleDebugInput(Camera2D& base_camera) {
    HandleDebugKeyboardInput(base_camera);
    HandleDebugMouseInput(base_camera);
}

void Game::HandleDebugKeyboardInput(Camera2D& base_camera) {
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
        base_camera.Translate(-Vector2::Y_AXIS * 10.0f);
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::J)) {
        base_camera.Translate(-Vector2::X_AXIS * 10.0f);
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::K)) {
        base_camera.Translate(Vector2::Y_AXIS * 10.0f);
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::L)) {
        base_camera.Translate(Vector2::X_AXIS * 10.0f);
    }
}

void Game::HandleDebugMouseInput(Camera2D& /*base_camera*/) {
    if(g_theUISystem->GetIO().WantCaptureMouse) {
        return;
    }
}
