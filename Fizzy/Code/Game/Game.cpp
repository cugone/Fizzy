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
    _test1.collider.SetSides(4);
    _test1.collider.AddPaddingToSides(25.0f, 25.0f);
    _test1.collider.Translate(Vector2(200.0f, 250.0f));
    _test2.collider.SetSides(4);
    _test2.collider.AddPaddingToSides(25.0f, 25.0f);
    _test2.collider.Translate(Vector2(400.0f, 250.0f));
    _test3.collider.SetSides(4);
    _test3.collider.AddPaddingToSides(50.0f, 50.0f);
    _test3.collider.Translate(Vector2(600.0f, 250.0f));
    _test4.collider.SetSides(4);
    _test4.collider.Translate(Vector2(800.0f, 250.0f));
    _test1.collider.SetOrientationDegrees(0.0f);
    _test2.collider.SetOrientationDegrees(0.0f);
    _test3.collider.SetOrientationDegrees(0.0f);
    _test4.collider.SetOrientationDegrees(0.0f);
    test_sides = _test5.GetSides();
	obbs.reserve(4);
	obbs.push_back(&_test1);
	obbs.push_back(&_test2);
	obbs.push_back(&_test3);
	obbs.push_back(&_test4);
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
    _test5.SetSides(test_sides);

    Camera2D& base_camera = _ui_camera;
    HandleDebugInput(deltaSeconds, base_camera);
    HandlePlayerInput(deltaSeconds, base_camera);
    base_camera.Update(deltaSeconds);

    _test2.collider = MathUtils::Interpolate(_test1.collider, _test3.collider, t);

    auto s = obbs.size();
	for (std::size_t i = 0; i < s; ++i) {
		obbs[i]->is_colliding = false;
	}
    for(std::size_t i = 0; i < s; ++i) {
        for(std::size_t j = i + 1; j < s; ++j) {
            obbs[i]->is_colliding |= MathUtils::DoPolygonsOverlap(obbs[i]->collider, obbs[j]->collider);
        }
    }
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
    g_theRenderer->DrawPolygon2D(_test1.collider, _test1.is_colliding ? Rgba::Red : Rgba::White);
    g_theRenderer->DrawPolygon2D(_test2.collider, _test2.is_colliding ? Rgba::Blue : Rgba::White);
    g_theRenderer->DrawPolygon2D(_test3.collider, _test3.is_colliding ? Rgba::Green : Rgba::White);
    g_theRenderer->DrawFilledCircle2D(_test4.collider.GetPosition(), 5.0f, _test4.is_colliding ? Rgba::Yellow : Rgba::White);
    g_theRenderer->DrawPolygon2D(_test5, Rgba::White);
    const auto f = g_theRenderer->GetFont("System32");
    const auto window_pos = g_theInputSystem->GetCursorWindowPosition(*g_theRenderer->GetOutput()->GetWindow());
    const auto world_pos = g_theRenderer->ConvertScreenToWorldCoords(window_pos);
    const auto T = Matrix4::CreateTranslationMatrix(Vector2{5.0f, f->GetLineHeight()});
    g_theRenderer->SetModelMatrix(T);
    std::ostringstream ss;
    ss << "Screen: "  << window_pos << '\n'
        << "World: "  << world_pos << '\n'
        << "AngleD: " << _test5.GetOrientationDegrees() << '\n'
        << "Pos: " << _test5.GetPosition();
    g_theRenderer->DrawMultilineText(g_theRenderer->GetFont("System32"), ss.str());

}

void Game::EndFrame() {
    /* DO NOTHING */
}

void Game::HandlePlayerInput(TimeUtils::FPSeconds deltaSeconds, Camera2D& /*base_camera*/) {

    if(g_theInputSystem->IsKeyDown(KeyCode::R)) {
        _test1.collider.SetOrientationDegrees(0.0f);
        _test3.collider.SetOrientationDegrees(0.0f);
    }

    float speed = 50.0f;
    if(g_theInputSystem->IsKeyDown(KeyCode::D)) {
        _test3.collider.Translate(Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::A)) {
        _test3.collider.Translate(-Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::W)) {
        _test3.collider.Translate(-Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::S)) {
        _test3.collider.Translate(Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::Backspace)) {
        _test3.collider.SetPosition(Vector2(225.0f, 225.0f));
    }

    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad6)) {
        _test1.collider.Translate(Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad4)) {
        _test1.collider.Translate(-Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad8)) {
        _test1.collider.Translate(-Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad5)) {
        _test1.collider.Translate(Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::Q)) {
        _test3.collider.Rotate(-45.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::E)) {
        _test3.collider.Rotate(+45.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad7)) {
        _test1.collider.Rotate(-45.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::NumPad9)) {
        _test1.collider.Rotate(+45.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::Right)) {
        _test4.collider.Translate(Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::Left)) {
        _test4.collider.Translate(-Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::Up)) {
        _test4.collider.Translate(-Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::Down)) {
        _test4.collider.Translate(Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::L)) {
        _test5.Translate(Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::J)) {
        _test5.Translate(-Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::I)) {
        _test5.Translate(-Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::K)) {
        _test5.Translate(Vector2::Y_AXIS * speed * deltaSeconds.count());
    }

    if(g_theInputSystem->IsKeyDown(KeyCode::U)) {
        _test5.Rotate(-45.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::O)) {
        _test5.Rotate(+45.0f * deltaSeconds.count());
    }

}

void Game::ShowDebugWindow() {
    if(ImGui::Begin("Debug Window", &_show_debug_window, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::SliderFloat("Interp", &t, 0.0f, 1.0f);
        ImGui::SliderInt("Sides", &test_sides, 3, 64);
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
