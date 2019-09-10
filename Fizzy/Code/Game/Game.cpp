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

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

void Game::Initialize() {
    g_theRenderer->RegisterMaterialsFromFolder(std::string{ "Data/Materials" });
    _test_AABB2 = AABB2::NEG_ONE_TO_ONE;
    _test_OBB2 = OBB2::ZERO_TO_ONE;
}

void Game::BeginFrame() {
    /* DO NOTHING */
}

void Game::Update(TimeUtils::FPSeconds deltaSeconds) {
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::Esc)) {
        g_theApp->SetIsQuitting(true);
        return;
    }
    //g_theInputSystem->SetCursorToWindowCenter(*g_theRenderer->GetOutput()->GetWindow());
    auto coords = g_theInputSystem->GetCursorWindowPosition(*g_theRenderer->GetOutput()->GetWindow());
    _test_AABB2 = AABB2::NEG_ONE_TO_ONE;
    _test_AABB2.AddPaddingToSides(50.0f, 50.0f);
    _test_AABB2 += coords;

    Camera2D& base_camera = _ui_camera;
    HandleDebugInput(base_camera);
    HandlePlayerInput(base_camera);
    base_camera.Update(deltaSeconds);
    g_theInputSystem->GetCursorScreenPosition();
    _test_OBB2.RotateDegrees(45.0f * deltaSeconds.count());

    _do_overlap = MathUtils::DoOBBsOverlap(_test_OBB2, _test_AABB2);
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
    auto ui_cam_pos = ui_view_half_extents;
    _ui_camera.position = ui_cam_pos;
    _ui_camera.orientation_degrees = 0.0f;
    _ui_camera.SetupView(ui_leftBottom, ui_rightTop, ui_nearFar, MathUtils::M_16_BY_9_RATIO);
    g_theRenderer->SetCamera(_ui_camera);

    Matrix4 T = Matrix4::I;
    Matrix4 R = Matrix4::I;
    Matrix4 S = Matrix4::I;
    Matrix4 M = T * R * S;
    g_theRenderer->SetModelMatrix(M);
    g_theRenderer->DrawAABB2(_test_AABB2, Rgba::Red, Rgba::NoAlpha);

    T = Matrix4::CreateTranslationMatrix(Vector2::ONE * 300.0f);
    R = Matrix4::Create2DRotationDegreesMatrix(_test_OBB2.orientationDegrees);
    S = Matrix4::CreateScaleMatrix(Vector2::ONE * 100.0f);
    M = T * R * S;
    g_theRenderer->SetModelMatrix(M);
    g_theRenderer->DrawOBB2(_test_OBB2.orientationDegrees, Rgba::Green, _do_overlap ? Rgba::Red : Rgba::NoAlpha);

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

void Game::HandleDebugKeyboardInput(Camera2D& /*base_camera*/) {
    if(g_theUISystem->GetIO().WantCaptureKeyboard) {
        return;
    }
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::F1)) {
        _show_debug_window = !_show_debug_window;
    }
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::F4)) {
        g_theUISystem->ToggleImguiDemoWindow();
    }
}

void Game::HandleDebugMouseInput(Camera2D& /*base_camera*/) {
    if(g_theUISystem->GetIO().WantCaptureMouse) {
        return;
    }
}
