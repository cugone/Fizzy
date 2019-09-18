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
    OnEnterState(0);
}

void Game::BeginFrame() {
    if(_next_state != _cur_state) {
        OnExitState(_cur_state);
        _cur_state = _next_state;
        OnEnterState(_cur_state);
    }
    switch(_cur_state) {
    case 0: BeginFrame_OBBs(); break;
    case 1: BeginFrame_Physics(); break;
    default: ERROR_AND_DIE("BEGIN FRAME UNDEFINED GAME STATE"); break;
    }
}

void Game::Update(TimeUtils::FPSeconds deltaSeconds) {
    switch(_cur_state) {
    case 0: Update_OBBs(deltaSeconds); break;
    case 1: Update_Physics(deltaSeconds); break;
    default: ERROR_AND_DIE("UPDATE UNDEFINED GAME STATE"); break;
    }
}

void Game::Render() const {
    RenderCommon();
    switch(_cur_state) {
    case 0: Render_OBBs(); break;
    case 1: Render_Physics(); break;
    default: ERROR_AND_DIE("RENDER UNDEFINED GAME STATE"); break;
    }
}

void Game::EndFrame() {
    switch(_cur_state) {
    case 0: EndFrame_OBBs(); break;
    case 1: EndFrame_Physics(); break;
    default: ERROR_AND_DIE("ENDFRAME UNDEFINED GAME STATE"); break;
    }
}

void Game::HandlePlayerInput_Physics(TimeUtils::FPSeconds /*deltaSeconds*/, Camera2D& /*base_camera*/) {
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::G)) {
        for(auto& b : _bodies) {
            b.EnableGravity(!b.IsGravityEnabled());
        }
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::Enter)) {
        const auto window_pos = g_theInputSystem->GetCursorWindowPosition(*g_theRenderer->GetOutput()->GetWindow());
        const auto displacement = _bodies[0].transform.GetTranslation().GetXY() - window_pos;
        const auto dir = displacement.GetNormalize();
        const auto magnitude = 10000.0f;
        _bodies[0].ApplyForce(dir * magnitude);
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::L)) {
        const auto window_pos = g_theInputSystem->GetCursorWindowPosition(*g_theRenderer->GetOutput()->GetWindow());
        const auto displacement = _bodies[0].transform.GetTranslation().GetXY() - window_pos;
        const auto dir = displacement.GetNormalize();
        const auto magnitude = 10000.0f;
        _bodies[0].ApplyForceAt(window_pos, dir * magnitude);
    }
}

void Game::HandlePlayerInput_OBBs(TimeUtils::FPSeconds deltaSeconds, Camera2D& /*base_camera*/) {
    auto& obb = _obbs[0];
    if(g_theInputSystem->IsKeyDown(KeyCode::R)) {
        Reset_OBBS();
    }

    float speed = 100.0f;
    if(g_theInputSystem->IsKeyDown(KeyCode::D)) {
        obb.Translate(Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::A)) {
        obb.Translate(-Vector2::X_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::W)) {
        obb.Translate(-Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::S)) {
        obb.Translate(Vector2::Y_AXIS * speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::Backspace)) {
        obb.position = Vector2(225.0f, 225.0f);
    }

    if(g_theInputSystem->IsKeyDown(KeyCode::Q)) {
        obb.SetOrientationDegrees(obb.orientationDegrees - 45.0f * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::E)) {
        obb.SetOrientationDegrees(obb.orientationDegrees + 45.0f * deltaSeconds.count());
    }

}

void Game::ChangeState(int newState) {
    _next_state = newState;
}

void Game::OnEnterState(int state) {
    switch(state) {
    case 0: OnEnter_OBBs(); break;
    case 1: OnEnter_Physics(); break;
    default: ERROR_AND_DIE("ON ENTER UNDEFINED GAME STATE") break;
    }
}

void Game::OnExitState(int state) {
    switch(state) {
    case 0: OnExit_OBBs(); break;
    case 1: OnExit_Physics(); break;
    default: ERROR_AND_DIE("ON ENTER UNDEFINED GAME STATE") break;
    }
}

void Game::OnEnter_OBBs() {
    Reset_OBBS();
}

void Game::OnEnter_Physics() {
    _bodies.resize(2);
    float width = static_cast<float>(g_theRenderer->GetOutput()->GetDimensions().x) - 200.0f;
    for(int i = 0; i < 2; ++i) {
        float x = std::fmod((i + 1) * 200.0f, width);
        float y = 250.0f;
        _bodies[i] = RigidBody{ { PhysicsMaterial{}
                    ,Vector2(x, y)
                    ,Vector2::ZERO
                    ,Vector2::ZERO
                    ,OBB2(Vector2(x, y), Vector2::ONE * 50.0f, 0.0f)
                    } };
        _bodies[i].EnableGravity(false);
        g_thePhysicsSystem->AddObject(_bodies[i]);
    }
    g_thePhysicsSystem->Enable(true);
    g_thePhysicsSystem->DebugShowCollision(true);
}

void Game::OnExit_OBBs() {
    _obbs.clear();
}

void Game::OnExit_Physics() {
    for(const auto& b : _bodies) {
        g_thePhysicsSystem->RemoveObject(b);
    }
    g_thePhysicsSystem->DebugShowCollision(false);
    g_thePhysicsSystem->Enable(false);
    _bodies.clear();
}

void Game::BeginFrame_OBBs() {
    for(auto& b : _obbs_colliding) {
        b = false;
    }
}

void Game::BeginFrame_Physics() {
    /* DO NOTHING */
}

void Game::Update_OBBs(TimeUtils::FPSeconds deltaSeconds) {
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
    HandlePlayerInput_OBBs(deltaSeconds, base_camera);
    base_camera.Update(deltaSeconds);

    for(auto i = 0; i < _obbs.size(); ++i) {
        for(auto j = i + 1; j < _obbs.size(); ++j) {
            _obbs_colliding[i] = MathUtils::DoOBBsOverlap(_obbs[i], _obbs[j]);
            _obbs_colliding[j] = MathUtils::DoOBBsOverlap(_obbs[i], _obbs[j]);
        }
    }
}

void Game::Update_Physics(TimeUtils::FPSeconds deltaSeconds) {
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
    HandlePlayerInput_Physics(deltaSeconds, base_camera);
    base_camera.Update(deltaSeconds);
}

void Game::Render_OBBs() const {
    RenderCommon();

    for(int i = 0; i < _obbs.size(); ++i) {
        const auto& obb = _obbs[i];
        g_theRenderer->DrawOBB2(obb, _obbs_colliding[i] ? Rgba::Red : Rgba::White);
    }
}

void Game::RenderCommon() const {
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

void Game::Render_Physics() const {
    const auto window_pos = g_theInputSystem->GetCursorWindowPosition(*g_theRenderer->GetOutput()->GetWindow());
    const auto closest_point = MathUtils::CalcClosestPoint(window_pos, _bodies[0].GetCollider());
    g_theRenderer->DrawFilledCircle2D(closest_point, 10.0f);
}

void Game::EndFrame_OBBs() {
    /* DO NOTHING */
}

void Game::EndFrame_Physics() {
    /* DO NOTHING */
}

void Game::ShowDebugWindow() {
    if(!ImGui::Begin("Debug Window", &_show_debug_window, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::End();
    }
    ImGui::End();
}

void Game::Reset_OBBS() {
    _obbs.resize(2);
    _obbs_colliding.resize(2);
    for(int i = 0; i < 2; ++i) {
        float x = (i + 1) * 200.0f;
        float y = 250.0f;
        _obbs[i] = OBB2(Vector2(x, y), Vector2::ONE * 50.0f, 0.0f);
    }
    for(auto& b : _obbs_colliding) {
        b = false;
    }
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
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::LeftBracket)) {
        ChangeState((_cur_state + 1) % 2);
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
