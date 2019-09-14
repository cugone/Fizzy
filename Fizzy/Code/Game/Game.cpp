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
    _test1.obb.AddPaddingToSides(25.0f, 25.0f);
    _test1.obb.Translate(Vector2(200.0f, 250.0f));
    _test2.obb.AddPaddingToSides(25.0f, 25.0f);
    _test2.obb.Translate(Vector2(400.0f, 250.0f));
    _test3.obb.AddPaddingToSides(50.0f, 50.0f);
    _test3.obb.Translate(Vector2(600.0f, 250.0f));
    _test1.obb.SetOrientationDegrees(0.0f);
    _test2.obb.SetOrientationDegrees(0.0f);
    _test3.obb.SetOrientationDegrees(0.0f);
    _closest_point.radius = 5.0f;
}

void Game::BeginFrame() {
    /* DO NOTHING */
}

void Game::Update(TimeUtils::FPSeconds deltaSeconds) {
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::Esc)) {
        g_theApp->SetIsQuitting(true);
        return;
    }

    if(_show_debug_window) {
        ShowDebugWindow();
    }

    Camera2D& base_camera = _ui_camera;
    HandleDebugInput(deltaSeconds, base_camera);
    HandlePlayerInput(deltaSeconds, base_camera);
    base_camera.Update(deltaSeconds);

    _test2.obb = MathUtils::Interpolate(_test1.obb, _test3.obb, t);

    const auto mouse_coords = g_theInputSystem->GetCursorWindowPosition(*g_theRenderer->GetOutput()->GetWindow());
    const auto world_pos = Vector2{g_theRenderer->ConvertScreenToWorldCoords(mouse_coords)};
    _test1.is_colliding = false;
    _test2.is_colliding = false;
    _test3.is_colliding = false;
    _test1.is_colliding |= MathUtils::DoOBBsOverlap(_test1.obb, _test2.obb);
    _test2.is_colliding |= MathUtils::DoOBBsOverlap(_test2.obb, _test1.obb);
    _test3.is_colliding |= MathUtils::DoOBBsOverlap(_test3.obb, _test1.obb);
    _test1.is_colliding |= MathUtils::DoOBBsOverlap(_test1.obb, _test3.obb);
    _test2.is_colliding |= MathUtils::DoOBBsOverlap(_test2.obb, _test3.obb);
    _test3.is_colliding |= MathUtils::DoOBBsOverlap(_test3.obb, _test2.obb);
    _closest_point.center = MathUtils::CalcClosestPoint(world_pos, _test3.obb);
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

    g_theRenderer->DrawAxes(static_cast<float>(std::max(ui_view_extents.x, ui_view_extents.y)), false);

    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__2D"));
    g_theRenderer->DrawOBB2(_test1.obb, _test1.is_colliding ? Rgba::Red : Rgba::White, Rgba::NoAlpha);
    g_theRenderer->DrawOBB2(_test2.obb, _test2.is_colliding ? Rgba::Blue : Rgba::White, Rgba::NoAlpha);
    g_theRenderer->DrawOBB2(_test3.obb, _test3.is_colliding ? Rgba::Green : Rgba::White, Rgba::NoAlpha);
    g_theRenderer->DrawFilledCircle2D(_closest_point);

    const auto f = g_theRenderer->GetFont("System32");
    const auto window_pos = g_theInputSystem->GetCursorWindowPosition(*g_theRenderer->GetOutput()->GetWindow());
    const auto world_pos = g_theRenderer->ConvertScreenToWorldCoords(window_pos);
    const auto T = Matrix4::CreateTranslationMatrix(Vector2{5.0f, f->GetLineHeight()});
    g_theRenderer->SetModelMatrix(T);
    std::ostringstream ss;
    ss << "Screen: " << window_pos << '\n' << "World: " << world_pos;
    g_theRenderer->DrawMultilineText(g_theRenderer->GetFont("System32"), ss.str());

}

void Game::EndFrame() {
    /* DO NOTHING */
}

void Game::HandlePlayerInput(TimeUtils::FPSeconds deltaSeconds, Camera2D& /*base_camera*/) {

    if(g_theInputSystem->IsKeyDown(KeyCode::R)) {
        _test1.obb.SetOrientationDegrees(0.0f);
        _test3.obb.SetOrientationDegrees(0.0f);
    }

    float speed = 50.0f;
    if(g_theInputSystem->IsKeyDown(KeyCode::D)) {
        _test3.obb.Translate(Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::A)) {
        _test3.obb.Translate(-Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::W)) {
        _test3.obb.Translate(-Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::S)) {
        _test3.obb.Translate(Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::Backspace)) {
        _test3.obb.position = Vector2(225.0f, 225.0f);
    }

    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad6)) {
        _test1.obb.Translate(Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad4)) {
        _test1.obb.Translate(-Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad8)) {
        _test1.obb.Translate(-Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad5)) {
        _test1.obb.Translate(Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::Q)) {
        _test3.obb.SetOrientationDegrees(_test3.obb.orientationDegrees - 45.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::E)) {
        _test3.obb.SetOrientationDegrees(_test3.obb.orientationDegrees + 45.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad7)) {
        _test1.obb.SetOrientationDegrees(_test1.obb.orientationDegrees - 45.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad9)) {
        _test1.obb.SetOrientationDegrees(_test1.obb.orientationDegrees + 45.0f * deltaSeconds.count());
    }

}

void Game::ShowDebugWindow() {
    if(ImGui::Begin("Debug Window", &_show_debug_window, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::SliderFloat("Interp", &t, 0.0f, 1.0f);
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
