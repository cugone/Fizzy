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
    case 0: BeginFrame_Physics(); break;
    default: ERROR_AND_DIE("BEGIN FRAME UNDEFINED GAME STATE"); break;
    }
}

void Game::Update(TimeUtils::FPSeconds deltaSeconds) {
    switch(_cur_state) {
    case 0: Update_Physics(deltaSeconds); break;
    default: ERROR_AND_DIE("UPDATE UNDEFINED GAME STATE"); break;
    }
}

void Game::Render() const {
    RenderCommon();
    switch(_cur_state) {
    case 0: Render_Physics(); break;
    default: ERROR_AND_DIE("RENDER UNDEFINED GAME STATE"); break;
    }
}

void Game::EndFrame() {
    switch(_cur_state) {
    case 0: EndFrame_Physics(); break;
    default: ERROR_AND_DIE("ENDFRAME UNDEFINED GAME STATE"); break;
    }
}

void Game::HandlePlayerInput_Physics(TimeUtils::FPSeconds /*deltaSeconds*/, Camera2D& /*base_camera*/) {
    if(g_theUISystem->GetIO().WantCaptureMouse) {
        return;
    }
    if(g_theUISystem->GetIO().WantCaptureKeyboard) {
        return;
    }
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::G)) {
        for(auto& b : _bodies) {
            b.EnableGravity(!b.IsGravityEnabled());
        }
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::Enter)) {
        const auto window_pos = g_theInputSystem->GetMouseCoords();
        const auto displacement = _bodies[0].GetPosition() - window_pos;
        const auto dir = displacement.GetNormalize();
        const auto magnitude = 10000.0f;
        _bodies[0].ApplyForce(dir * magnitude);
    }
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::LButton)) {
        const auto mouse_pos = g_theInputSystem->GetMouseCoords();
        _new_bodies.push_back(mouse_pos);
    }
}

void Game::ChangeState(int newState) {
    _next_state = newState;
}

void Game::OnEnterState(int state) {
    switch(state) {
    case 0: OnEnter_Physics(); break;
    default: ERROR_AND_DIE("ON ENTER UNDEFINED GAME STATE") break;
    }
}

void Game::OnExitState(int state) {
    switch(state) {
    case 0: OnExit_Physics(); break;
    default: ERROR_AND_DIE("ON ENTER UNDEFINED GAME STATE") break;
    }
}

void Game::OnEnter_Physics() {
    float width = static_cast<float>(g_theRenderer->GetOutput()->GetDimensions().x) - 100.0f;
    //float height = static_cast<float>(g_theRenderer->GetOutput()->GetDimensions().y) - 200.0f;
    _bodies.reserve(20);
    for(int i = 0; i < 20; ++i) {
        float x = 100.0f + std::fmod((i + 1) * 200.0f, width);
        float y = 50.0f + ((200.0f * i) / width);
        _bodies.push_back(RigidBody{ RigidBodyDesc{ PhysicsMaterial{}
                    ,Vector2(x, y)
                    ,Vector2::ZERO
                    ,Vector2::ZERO
                    , std::move(std::make_unique<ColliderCircle>(Vector2(x, y), 25.0f))
                    } });
        _bodies.back().EnableGravity(false);
    }
    std::vector<RigidBody*> body_ptrs(_bodies.size());
    for(std::size_t i = 0u; i < _bodies.size(); ++i) {
        body_ptrs[i] = &_bodies[i];
    }
    g_thePhysicsSystem->AddObjects(body_ptrs);
    g_thePhysicsSystem->SetWorldDescription({AABB2(-Vector2(g_theRenderer->GetOutput()->GetDimensions()), Vector2(g_theRenderer->GetOutput()->GetDimensions())) });
    g_thePhysicsSystem->Enable(true);
    g_thePhysicsSystem->DebugShowCollision(true);
}

void Game::OnExit_Physics() {
    for(const auto& b : _bodies) {
        g_thePhysicsSystem->RemoveObject(&b);
    }
    g_thePhysicsSystem->DebugShowCollision(false);
    g_thePhysicsSystem->Enable(false);
    _bodies.clear();
}

void Game::BeginFrame_Physics() {
    /* DO NOTHING */
}

void Game::Update_Physics(TimeUtils::FPSeconds deltaSeconds) {
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::Esc)) {
        g_theApp->SetIsQuitting(true);
        return;
    }
    g_thePhysicsSystem->DebugShowWorldPartition(_show_world_partition);
    g_thePhysicsSystem->DebugShowCollision(_show_collision);
    g_theRenderer->UpdateGameTime(deltaSeconds);
    if(_show_debug_window) {
        ShowDebugWindow();
    }

    Camera2D& base_camera = _ui_camera;
    HandleDebugInput(deltaSeconds, base_camera);
    HandlePlayerInput_Physics(deltaSeconds, base_camera);
    base_camera.Update(deltaSeconds);
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
    /* DO NOTHING */
}

void Game::EndFrame_Physics() {
    for(const auto& pos : _new_bodies) {
        _bodies.push_back(RigidBody{ RigidBodyDesc{ PhysicsMaterial{}
            ,pos
            ,Vector2::ZERO
            ,Vector2::ZERO
            , std::move(std::make_unique<ColliderCircle>(pos, 25.0f))
            } });
        g_thePhysicsSystem->AddObject(&_bodies.back());
    }
    _new_bodies.clear();
}

void Game::ShowDebugWindow() {
    if(ImGui::Begin("Debug Window", &_show_debug_window, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Checkbox("Show Quadtree", &_show_world_partition);
        ImGui::Checkbox("Show Collision", &_show_collision);
        if(!_bodies.empty()) {
            const auto [distance, is_valid] = GJKDistance(*_bodies[0].GetCollider(), *_bodies[1].GetCollider());
            if(is_valid) {
                ImGui::Text("GJKDistance: %f", distance);
            } else {
                ImGui::Text("GJKDistance: Invalid");
            }
        } else {
            ImGui::Text("GJKDistance: Invalid");
        }
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
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::LeftBracket)) {
        ChangeState((_cur_state + 1) % 2);
    }
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::F4)) {
        g_theUISystem->ToggleImguiDemoWindow();
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::I)) {
        base_camera.Translate(-Vector2::Y_AXIS * _cam_speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::J)) {
        base_camera.Translate(-Vector2::X_AXIS * _cam_speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::K)) {
        base_camera.Translate(Vector2::Y_AXIS * _cam_speed * deltaSeconds.count());
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::L)) {
        base_camera.Translate(Vector2::X_AXIS * _cam_speed * deltaSeconds.count());
    }
}

void Game::HandleDebugMouseInput(TimeUtils::FPSeconds /*deltaSeconds*/, Camera2D& /*base_camera*/) {
    if(g_theUISystem->GetIO().WantCaptureMouse) {
        return;
    }
}
