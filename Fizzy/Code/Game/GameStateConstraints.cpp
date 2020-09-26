#include "Game/GameStateConstraints.hpp"

#include "Engine/Physics/PhysicsUtils.hpp"
#include "Engine/Physics/SpringJoint.hpp"
#include "Engine/Physics/RodJoint.hpp"

#include "Engine/Renderer/Window.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

#include <string>
#include <vector>

void GameStateConstraints::OnEnter() noexcept {
    float width = static_cast<float>(g_theRenderer->GetOutput()->GetDimensions().x);
    float height = static_cast<float>(g_theRenderer->GetOutput()->GetDimensions().y);
    const std::size_t maxBodies = 2;
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
    float x2 = x1 + 55.0f;
    float y2 = y1;
    float x3 = x2 + 55.0f;
    float y3 = y2;
    float x4 = x3 + 55.0f;
    float y4 = y3;
    float radius = 0.25f;
    _bodies.push_back(RigidBody(g_thePhysicsSystem, RigidBodyDesc(
                    Vector2(x1, y1)
                    ,Vector2::ZERO
                    ,Vector2::ZERO
                    ,new ColliderCircle(Vector2(x1, y1), radius)
                    ,PhysicsMaterial{0.0f, 0.0f, 0.0f}
                    ,PhysicsDesc{}
                    )));
    _bodies.push_back(RigidBody(g_thePhysicsSystem, RigidBodyDesc(
        Vector2(x2, y2)
        , Vector2::ZERO
        , Vector2::ZERO
        , new ColliderCircle(Vector2(x2, y2), radius)
        , PhysicsMaterial{0.0f, 0.0f}
        , PhysicsDesc{0.0f}
    )));
    _bodies.back().EnableGravity(true);
    _bodies.back().EnableDrag(false);

    _bodies.push_back(RigidBody(g_thePhysicsSystem, RigidBodyDesc(
        Vector2(x3, y3)
        , Vector2::ZERO
        , Vector2::ZERO
        , new ColliderCircle(Vector2(x3, y3), radius)
        , PhysicsMaterial{0.0f, 0.0f}
        , PhysicsDesc{0.0f}
    )));
    _bodies.back().EnableGravity(false);
    _bodies.back().EnableDrag(false);

    _bodies.push_back(RigidBody(g_thePhysicsSystem, RigidBodyDesc(
        Vector2(x4, y4)
        , Vector2::ZERO
        , Vector2::ZERO
        , new ColliderCircle(Vector2(x4, y4), radius)
        , PhysicsMaterial{0.0f, 0.0f}
        , PhysicsDesc{0.0f}
    )));
    _bodies.back().EnableGravity(true);
    _bodies.back().EnableDrag(false);


    std::vector<RigidBody*> body_ptrs(_bodies.size());
    for(std::size_t i = 0u; i < _bodies.size(); ++i) {
        body_ptrs[i] = &_bodies[i];
    }
    g_thePhysicsSystem->SetWorldDescription(physicsSystemDesc);
    g_thePhysicsSystem->AddObjects(body_ptrs);
    _activeBody = &_bodies[1];
    auto* sp_joint = g_thePhysicsSystem->CreateJoint<SpringJoint>(&_bodies[0], &_bodies[1]);
    sp_joint->SetRestingLength((Vector2{x1, y1} - Vector2{x2, y2}).CalcLength());
    sp_joint->SetAnchors(Vector2{x1, y1}, Vector2{x2, y2});
    sp_joint->SetStiffness(1.0f);
    g_thePhysicsSystem->CreateJoint<RodJoint>(&_bodies[2], &_bodies[3]);    
    g_thePhysicsSystem->Enable(true);
    g_thePhysicsSystem->DebugShowCollision(true);
}

void GameStateConstraints::OnExit() noexcept {
    g_thePhysicsSystem->RemoveAllObjectsImmediately();
    g_thePhysicsSystem->DebugShowCollision(false);
    g_thePhysicsSystem->Enable(false);
    _bodies.clear();
}


void GameStateConstraints::BeginFrame() noexcept {
    /* DO NOTHING */
}

void GameStateConstraints::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::Esc)) {
        g_theApp->SetIsQuitting(true);
        return;
    }
    g_thePhysicsSystem->DebugShowWorldPartition(_show_world_partition);
    g_thePhysicsSystem->DebugShowCollision(_show_collision);
    g_thePhysicsSystem->DebugShowJoints(_show_joints);
    g_theRenderer->UpdateGameTime(deltaSeconds);
    if(_show_debug_window) {
        ShowDebugWindow();
    }

    Camera2D& base_camera = _ui_camera;
    base_camera.Update(deltaSeconds);

    _debug_point_on_body = MathUtils::CalcClosestPoint(g_theInputSystem->GetMouseCoords(), *_activeBody->GetCollider());
    HandleInput();
}

void GameStateConstraints::Render() const noexcept {
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

    g_theRenderer->DrawAxes(static_cast<float>((std::max)(ui_view_extents.x, ui_view_extents.y)), false);
    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__2D"));

    if(!_debug_click_adds_bodies) {
        g_theRenderer->DrawFilledCircle2D(_debug_point_on_body, 5.0f);
    }

}

void GameStateConstraints::EndFrame() noexcept {
    /* DO NOTHING */
}

void GameStateConstraints::HandleInput() noexcept {
    HandleKeyboardInput();
    HandleMouseInput();
}

void GameStateConstraints::HandleKeyboardInput() noexcept {
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::F1)) {
        ToggleShowDebugWindow();
    }
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::F4)) {
        g_theUISystem->ToggleImguiDemoWindow();
    }
}

void GameStateConstraints::HandleMouseInput() noexcept {
    if(g_theUISystem->WantsInputMouseCapture()) {
        return;
    }
    Debug_AddBodyOrApplyForceAtMouseCoords();
}

void GameStateConstraints::ToggleShowDebugWindow() noexcept {
    _show_debug_window = !_show_debug_window;
}

void GameStateConstraints::Debug_AddBodyOrApplyForceAtMouseCoords() noexcept {
    if(_debug_click_adds_bodies) {
        if(g_theInputSystem->WasKeyJustPressed(KeyCode::LButton)) {
            Debug_AddBodyAtMouseCoords();
        }
    } else {
        if(g_theInputSystem->IsKeyDown(KeyCode::LButton)) {
            Debug_ApplyImpulseAtMouseCoords();
        }
    }
}

void GameStateConstraints::Debug_AddBodyAtMouseCoords() noexcept {
    const auto p = g_theInputSystem->GetMouseCoords();
    _new_body_positions.push_back(p);
}

void GameStateConstraints::Debug_ApplyImpulseAtMouseCoords() noexcept {
    const auto p = g_theInputSystem->GetMouseCoords();
    const auto point_on_body = MathUtils::CalcClosestPoint(p, *_activeBody->GetCollider());
    const auto direction = (point_on_body - p).GetNormalize();
    _activeBody->ApplyImpulse(direction * 100.0f);
}

void GameStateConstraints::ShowDebugWindow() {
    if(ImGui::Begin("Debug Window", &_show_debug_window)) {
        ImGui::Checkbox("Show Collision", &_show_collision);
        ImGui::Checkbox("Show Joints", &_show_joints);
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
        {
            std::string header = std::string{"Bodies - "} + std::to_string(b_size);
            if(ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                for(std::size_t i = 0; i < b_size; ++i) {
                    const auto* body = &_bodies[i];
                    if(ImGui::TreeNode(reinterpret_cast<void*>(static_cast<std::intptr_t>(i)), "Body %d", i)) {
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
                        ImGui::TreePop();
                    }
                }
            }
        }
        ImGui::End();
    }
}
