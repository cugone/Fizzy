#include "Game/GameStateSleepManagement.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

void GameStateSleepManagement::OnEnter() noexcept {
    float width = static_cast<float>(g_theRenderer->GetOutput()->GetDimensions().x);
    float height = static_cast<float>(g_theRenderer->GetOutput()->GetDimensions().y);
    const std::size_t maxBodies = 5;
    _bodies.clear();
    _bodies.reserve(maxBodies);
    float screenX = width * 0.50f;
    float screenY = height * 0.50f;
    const auto world_dims = g_theRenderer->GetOutput()->GetDimensions();
    const auto mins = Vector2(-world_dims) * 0.5f;
    const auto maxs = Vector2(world_dims) * 0.5f;
    auto physicsSystemDesc = PhysicsSystemDesc{};
    physicsSystemDesc.world_bounds = AABB2{mins, maxs};
    float x1 = screenX;
    float y1 = screenY;
    float x2 = x1 - 55.0f;
    float y2 = y1;
    float x3 = x1 + 55.0f;
    float y3 = y1;
    float x4 = x3 + 55.0f;
    float y4 = y1;
    float x5 = x4 + 55.0f;
    float y5 = y1;
    float radius = 25.0f;
    _bodies.push_back(RigidBody(g_thePhysicsSystem, RigidBodyDesc(
        Position{x2, y2}
        , Velocity{}
        , Acceleration{}
        , new ColliderCircle(Position{x2, y2}, radius)
        , PhysicsMaterial{0.0f, 0.0f}
        , PhysicsDesc{0.0f}
    )));
    _bodies.back().EnableGravity(false);
    _bodies.back().EnableDrag(false);
    _bodies.push_back(RigidBody(g_thePhysicsSystem, RigidBodyDesc(
        Position{x1, y1}
        , Velocity{}
        , Acceleration{}
        , new ColliderCircle(Vector2(x1, y1), radius)
        , PhysicsMaterial{0.0f, 0.0f, 0.0f}
        , PhysicsDesc{}
    )));
    _bodies.back().EnableGravity(true);
    _bodies.back().EnableDrag(false);
    _bodies.push_back(RigidBody(g_thePhysicsSystem, RigidBodyDesc(
        Position{x3, y3}
        , Velocity{}
        , Acceleration{}
        , new ColliderCircle(Vector2(x3, y3), radius)
        , PhysicsMaterial{0.0f, 0.0f}
        , PhysicsDesc{}
    )));
    _bodies.back().EnableGravity(false);
    _bodies.back().EnableDrag(true);
    _bodies.push_back(RigidBody(g_thePhysicsSystem, RigidBodyDesc(
        Position{x4, y4}
        , Velocity{}
        , Acceleration{}
        , new ColliderCircle(Vector2(x4, y4), radius)
        , PhysicsMaterial{0.0f, 0.0f}
        , PhysicsDesc{}
    )));
    _bodies.back().EnableGravity(true);
    _bodies.back().EnableDrag(true);
    _bodies.push_back(RigidBody(g_thePhysicsSystem, RigidBodyDesc(
        Position{x5, y5}
        , Velocity{}
        , Acceleration{}
        , new ColliderPolygon(3, Vector2(x5, y5), Vector2{radius, radius} *2.0f, 0.0f)
        , PhysicsMaterial{0.0f, 0.0f}
        , PhysicsDesc{}
    )));
    _bodies.back().EnableGravity(false);
    _bodies.back().EnableDrag(false);
    std::vector<RigidBody*> body_ptrs(_bodies.size());
    for(std::size_t i = 0u; i < _bodies.size(); ++i) {
        body_ptrs[i] = &_bodies[i];
    }
    g_thePhysicsSystem->SetWorldDescription(physicsSystemDesc);
    g_thePhysicsSystem->AddObjects(body_ptrs);

    g_thePhysicsSystem->Enable(true);
    g_thePhysicsSystem->Debug_ShowCollision(true);
}

void GameStateSleepManagement::OnExit() noexcept {
    g_thePhysicsSystem->RemoveAllObjectsImmediately();
    g_thePhysicsSystem->Debug_ShowCollision(false);
    g_thePhysicsSystem->Enable(false);
    _bodies.clear();
}

void GameStateSleepManagement::BeginFrame() noexcept {
    /* DO NOTHING */
}

void GameStateSleepManagement::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::Esc)) {
        g_theApp->SetIsQuitting(true);
        return;
    }
    g_thePhysicsSystem->Debug_ShowWorldPartition(_show_world_partition);
    g_thePhysicsSystem->Debug_ShowCollision(_show_collision);
    g_theRenderer->UpdateGameTime(deltaSeconds);
    if(_show_debug_window) {
        ShowDebugWindow();
    }

    Camera2D& base_camera = _ui_camera;
    base_camera.Update(deltaSeconds);

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
        if(ImGui::Button("Fire projectile")) {
            FireProjectile();
        }
    }
    ImGui::End();
}

void GameStateSleepManagement::ToggleShowDebugWindow() noexcept {
    _show_debug_window = !_show_debug_window;
}

void GameStateSleepManagement::FireProjectile() noexcept {
    
}

