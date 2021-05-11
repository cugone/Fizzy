#include "Game/GameStateGravityDrag.hpp"

#include "Engine/Physics/PhysicsUtils.hpp"
#include "Engine/Physics/SpringJoint.hpp"

#include "Engine/Renderer/Window.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

void GameStateGravityDrag::OnEnter() noexcept {
    float width = static_cast<float>(g_theRenderer->GetOutput()->GetDimensions().x);
    float height = static_cast<float>(g_theRenderer->GetOutput()->GetDimensions().y);
    const std::size_t maxBodies = 2;
    _bodies.clear();
    _bodies.reserve(maxBodies);
    float screenX = width * 0.50f;
    float screenY = height * 0.50f;
    const auto world_dims = g_theRenderer->GetOutput()->GetDimensions();
    const auto mins = a2de::Vector2(-world_dims) * 0.5f;
    const auto maxs = a2de::Vector2(world_dims) * 0.5f;
    auto physicsSystemDesc = a2de::PhysicsSystemDesc{};
    physicsSystemDesc.world_bounds = a2de::AABB2{mins, maxs};
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
    _bodies.push_back(a2de::RigidBody(g_thePhysicsSystem, a2de::RigidBodyDesc(
        a2de::Position{x2, y2}
                    , a2de::Velocity{}
                    , a2de::Acceleration{}
        , new a2de::ColliderCircle(a2de::Position{x2, y2}, radius)
                    , a2de::PhysicsMaterial{0.0f, 0.0f}
                    , a2de::PhysicsDesc{0.0f}
                    )));
    _bodies.back().EnableGravity(false);
    _bodies.back().EnableDrag(false);
    _bodies.push_back(a2de::RigidBody(g_thePhysicsSystem, a2de::RigidBodyDesc(
        a2de::Position{x1, y1}
                    , a2de::Velocity{}
                    , a2de::Acceleration{}
                    , new a2de::ColliderCircle(a2de::Vector2(x1, y1), radius)
                    , a2de::PhysicsMaterial{0.0f, 0.0f, 0.0f}
                    , a2de::PhysicsDesc{}
                    )));
    _bodies.back().EnableGravity(true);
    _bodies.back().EnableDrag(false);
    _bodies.push_back(a2de::RigidBody(g_thePhysicsSystem, a2de::RigidBodyDesc(
        a2de::Position{x3, y3}
                , a2de::Velocity{}
                , a2de::Acceleration{}
                , new a2de::ColliderCircle(a2de::Vector2(x3, y3), radius)
                , a2de::PhysicsMaterial{0.0f, 0.0f}
                , a2de::PhysicsDesc{}
                )));
    _bodies.back().EnableGravity(false);
    _bodies.back().EnableDrag(true);
    _bodies.push_back(a2de::RigidBody(g_thePhysicsSystem, a2de::RigidBodyDesc(
        a2de::Position{x4, y4}
                , a2de::Velocity{}
                , a2de::Acceleration{}
                ,new a2de::ColliderCircle(a2de::Vector2(x4, y4), radius)
                , a2de::PhysicsMaterial{0.0f, 0.0f}
                , a2de::PhysicsDesc{}
                )));
    _bodies.back().EnableGravity(true);
    _bodies.back().EnableDrag(true);
    _bodies.push_back(a2de::RigidBody(g_thePhysicsSystem, a2de::RigidBodyDesc(
        a2de::Position{x5, y5}
        , a2de::Velocity{}
        , a2de::Acceleration{}
        , new a2de::ColliderPolygon(3, a2de::Vector2(x5, y5), a2de::Vector2{radius, radius} * 2.0f, 0.0f)
        , a2de::PhysicsMaterial{0.0f, 0.0f}
        , a2de::PhysicsDesc{}
    )));
    _bodies.back().EnableGravity(false);
    _bodies.back().EnableDrag(false);
    std::vector<a2de::RigidBody*> body_ptrs(_bodies.size());
    for(std::size_t i = 0u; i < _bodies.size(); ++i) {
        body_ptrs[i] = &_bodies[i];
    }
    g_thePhysicsSystem->SetWorldDescription(physicsSystemDesc);
    g_thePhysicsSystem->AddObjects(body_ptrs);
    _activeBody = &_bodies[2];
    if(_selected_body >= _bodies.size()) {
        _selected_body = 0u;
    }
    g_thePhysicsSystem->Enable(true);
    g_thePhysicsSystem->Debug_ShowCollision(true);
}

void GameStateGravityDrag::OnExit() noexcept {
    g_thePhysicsSystem->RemoveAllObjectsImmediately();
    g_thePhysicsSystem->Debug_ShowCollision(false);
    g_thePhysicsSystem->Enable(false);
    _bodies.clear();
}


void GameStateGravityDrag::BeginFrame() noexcept {
    /* DO NOTHING */
}

void GameStateGravityDrag::Update([[maybe_unused]] a2de::TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(g_theInputSystem->WasKeyJustPressed(a2de::KeyCode::Esc)) {
        g_theApp->SetIsQuitting(true);
        return;
    }
    g_thePhysicsSystem->Debug_ShowWorldPartition(_show_world_partition);
    g_thePhysicsSystem->Debug_ShowCollision(_show_collision);
    g_theRenderer->UpdateGameTime(deltaSeconds);
    if(_show_debug_window) {
        ShowDebugWindow();
    }

    a2de::Camera2D& base_camera = _ui_camera;
    base_camera.Update(deltaSeconds);

    _debug_point_on_body = a2de::MathUtils::CalcClosestPoint(g_theInputSystem->GetMouseCoords(), *_activeBody->GetCollider());
    HandleInput();
}

void GameStateGravityDrag::Render() const noexcept {
    g_theRenderer->ResetModelViewProjection();
    g_theRenderer->SetRenderTargetsToBackBuffer();
    g_theRenderer->ClearDepthStencilBuffer();

    g_theRenderer->ClearColor(a2de::Rgba::Black);

    g_theRenderer->SetViewportAsPercent();

    //2D View / HUD
    const auto& ui_view_height = currentGraphicsOptions.WindowHeight;
    const auto ui_view_width = ui_view_height * _ui_camera.GetAspectRatio();
    const auto ui_view_extents = a2de::Vector2{ui_view_width, ui_view_height};
    const auto ui_view_half_extents = ui_view_extents * 0.5f;
    auto ui_leftBottom = a2de::Vector2{-ui_view_half_extents.x, ui_view_half_extents.y};
    auto ui_rightTop = a2de::Vector2{ui_view_half_extents.x, -ui_view_half_extents.y};
    auto ui_nearFar = a2de::Vector2{0.0f, 1.0f};
    _ui_camera.position = ui_view_half_extents;
    _ui_camera.SetupView(ui_leftBottom, ui_rightTop, ui_nearFar, a2de::MathUtils::M_16_BY_9_RATIO);
    g_theRenderer->SetCamera(_ui_camera);

    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__2D"));
    g_theRenderer->DrawAxes(static_cast<float>((std::max)(ui_view_extents.x, ui_view_extents.y)), false);

    if(!_debug_click_adds_bodies) {
        g_theRenderer->DrawFilledCircle2D(_debug_point_on_body, 5.0f);
    }

}

void GameStateGravityDrag::EndFrame() noexcept {
    if(_new_body_positions.empty()) {
        return;
    }
    for(const auto& pos : _new_body_positions) {
        _bodies.push_back(a2de::RigidBody(g_thePhysicsSystem, a2de::RigidBodyDesc(
            pos
            , a2de::Vector2::ZERO
            , a2de::Vector2::ZERO
            , new a2de::ColliderCircle(pos, 25.0f)
            , a2de::PhysicsMaterial{}
            , a2de::PhysicsDesc{}
        )));
    }
    const auto new_size = _bodies.size();
    auto new_body_ptrs = std::vector<a2de::RigidBody*>(new_size);
    for(auto i = std::size_t{0u}; i < new_size; ++i) {
        new_body_ptrs[i] = &_bodies[i];
    }
    _activeBody = &_bodies[2];
    g_thePhysicsSystem->RemoveAllObjectsImmediately();
    g_thePhysicsSystem->AddObjects(new_body_ptrs);
    _new_body_positions.clear();
}

void GameStateGravityDrag::HandleInput() noexcept {
    HandleKeyboardInput();
    HandleMouseInput();
}

void GameStateGravityDrag::HandleKeyboardInput() noexcept {
    if(g_theInputSystem->WasKeyJustPressed(a2de::KeyCode::F1)) {
        ToggleShowDebugWindow();
    }
    if(g_theInputSystem->WasKeyJustPressed(a2de::KeyCode::F4)) {
        g_theUISystem->ToggleImguiDemoWindow();
    }
}

void GameStateGravityDrag::HandleMouseInput() noexcept {
    if(g_theUISystem->WantsInputMouseCapture()) {
        return;
    }
    Debug_AddBodyOrApplyForceAtMouseCoords();
}

void GameStateGravityDrag::ToggleShowDebugWindow() noexcept {
    _show_debug_window = !_show_debug_window;
}

void GameStateGravityDrag::Debug_AddBodyOrApplyForceAtMouseCoords() noexcept {
    if(_debug_click_adds_bodies) {
        if(g_theInputSystem->WasKeyJustPressed(a2de::KeyCode::LButton)) {
            Debug_AddBodyAtMouseCoords();
        }
    } else {
        if(g_theInputSystem->IsKeyDown(a2de::KeyCode::LButton)) {
            Debug_ApplyImpulseAtMouseCoords();
        }
    }
}

void GameStateGravityDrag::Debug_AddBodyAtMouseCoords() noexcept {
    const auto p = g_theInputSystem->GetMouseCoords();
    _new_body_positions.push_back(p);
}

void GameStateGravityDrag::Debug_ApplyImpulseAtMouseCoords() noexcept {
    const auto p = g_theInputSystem->GetMouseCoords();
    const auto point_on_body = a2de::MathUtils::CalcClosestPoint(p, *_activeBody->GetCollider());
    const auto direction = (point_on_body - p).GetNormalize();
    _activeBody->ApplyImpulse(direction * 150.0f);
}

void GameStateGravityDrag::ShowDebugWindow() {
    if(ImGui::Begin("Debug Window", &_show_debug_window)) {
        ImGui::Checkbox("Click adds bodies", &_debug_click_adds_bodies);
        ImGui::Checkbox("Show Quadtree", &_show_world_partition);
        ImGui::Checkbox("Show Collision", &_show_collision);
        Debug_SelectedBodiesComboBoxUI();
        Debug_ShowBodiesUI();
    }
    ImGui::End();
}

void GameStateGravityDrag::Debug_SelectedBodiesComboBoxUI() {
    const auto b_size = _bodies.size();
    std::vector<std::string> items{};
    items.resize(b_size);
    for(std::size_t i = 0u; i < b_size; ++i) {
        items[i] = std::string{"Body "} + std::to_string(i);
    }
    std::string current_item = items[_selected_body];
    if(ImGui::BeginCombo("Selected Body", current_item.c_str())) {
        for(auto it = std::cbegin(items); it != std::cend(items); ++it) {
            bool is_selected = current_item == *it;
            if(ImGui::Selectable((*it).c_str(), is_selected)) {
                current_item = *it;
                _selected_body = std::distance(std::cbegin(items), it);
            }
            if(is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    _activeBody = &_bodies[_selected_body];
}

void GameStateGravityDrag::Debug_ShowBodiesUI() {
    const auto b_size = _bodies.size();
    std::string header = std::string{"Bodies - "} + std::to_string(b_size);
    if(ImGui::CollapsingHeader(header.c_str())) {
        for(std::size_t i = 0; i < b_size; ++i) {
            const auto* body = &_bodies[i];
            if(ImGui::TreeNode(reinterpret_cast<void*>(static_cast<std::intptr_t>(i)), "Body %d", i)) {
                Debug_ShowBodyParametersUI(body);
                ImGui::TreePop();
            }
        }
    }
}

void GameStateGravityDrag::Debug_ShowBodyParametersUI(const a2de::RigidBody* const body) {
    const auto acc = body->GetAcceleration();
    const auto vel = body->GetVelocity();
    const auto pos = body->GetPosition();
    const auto aacc = body->GetAngularAccelerationDegrees();
    const auto avel = body->GetAngularVelocityDegrees();
    const auto apos = body->GetOrientationDegrees();
    const auto mass = body->GetMass();
    ImGui::Text("Awake: %s", (body->IsAwake() ? "true" : "false"));
    ImGui::Text("M: %f", mass);
    ImGui::Text("A: [%f, %f]", acc.x, acc.y);
    ImGui::Text("V: [%f, %f]", vel.x, vel.y);
    ImGui::Text("P: [%f, %f]", pos.x, pos.y);
    ImGui::Text("oA: %f", aacc);
    ImGui::Text("oV: %f", avel);
    ImGui::Text("oP: %f", apos);
}
