#include "Game/GameStateConstraints.hpp"

#include "Engine/Physics/PhysicsUtils.hpp"
#include "Engine/Physics/SpringJoint.hpp"
#include "Engine/Physics/RodJoint.hpp"
#include "Engine/Physics/CableJoint.hpp"

#include "Engine/Renderer/Window.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

#include <functional>
#include <string>
#include <vector>

void GameStateConstraints::OnEnter() noexcept {
    float width = static_cast<float>(g_theRenderer->GetOutput()->GetDimensions().x);
    float height = static_cast<float>(g_theRenderer->GetOutput()->GetDimensions().y);
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
    float x5 = x4 + 55.0f;
    float y5 = y4;
    float x6 = x5 + 25.0f;
    float y6 = y5 - 110.0f;
    float radius = 25.0f;
    _bodies.push_back(RigidBody(g_thePhysicsSystem, RigidBodyDesc(
        Position{x1, y1}
                    , Velocity{}
                    , Acceleration{}
                    ,new ColliderCircle(Vector2(x1, y1), radius)
                    , PhysicsMaterial{0.0f, 0.0f}
                    , PhysicsDesc{}
                    )));
    _bodies.push_back(RigidBody(g_thePhysicsSystem, RigidBodyDesc(
        Position(x2, y2)
        , Velocity{}
        , Acceleration{}
        , new ColliderCircle(Vector2(x2, y2), radius)
        , PhysicsMaterial{0.0f, 0.0f}
        , PhysicsDesc{0.0f}
    )));
    _bodies.back().EnableGravity(true);
    _bodies.back().EnableDrag(false);

    _bodies.push_back(RigidBody(g_thePhysicsSystem, RigidBodyDesc(
        Position{x3, y3}
        , Velocity{}
        , Acceleration{}
        , new ColliderCircle(Vector2(x3, y3), radius)
        , PhysicsMaterial{0.0f, 0.0f, 0.0f}
        , PhysicsDesc{0.0f}
    )));
    _bodies.back().EnableGravity(false);
    _bodies.back().EnableDrag(false);

    _bodies.push_back(RigidBody(g_thePhysicsSystem, RigidBodyDesc(
        Position{x4, y4}
        , Velocity{}
        , Acceleration{}
        , new ColliderCircle(Vector2(x4, y4), radius)
        , PhysicsMaterial{0.0f, 0.0f, 5.0f}
        , PhysicsDesc{0.0f}
    )));
    _bodies.back().EnableGravity(true);
    _bodies.back().EnableDrag(false);


    _bodies.push_back(RigidBody(g_thePhysicsSystem, RigidBodyDesc(
        Position{x5, y5}
        , Velocity{}
        , Acceleration{}
        , new ColliderCircle(Vector2(x5, y5), radius)
        , PhysicsMaterial{0.0f, 0.0f, 0.0f}
        , PhysicsDesc{0.0f}
    )));
    _bodies.back().EnableGravity(false);
    _bodies.back().EnableDrag(false);

    _bodies.push_back(RigidBody(g_thePhysicsSystem, RigidBodyDesc(
        Position{x6, y6}
        , Velocity{}
        , Acceleration{}
        , new ColliderCircle(Vector2(x6, y6), radius)
        , PhysicsMaterial{0.0f, 0.0f, 10.0f}
        , PhysicsDesc{0.0f}
    )));
    _bodies.back().EnableGravity(true);
    _bodies.back().EnableDrag(false);

    _activeBody = &_bodies[0];

    {
        SpringJointDef spring{};
        spring.rigidBodyA = &_bodies[0];
        spring.rigidBodyB = &_bodies[1];
        spring.k = 1.0f;
        spring.length = 55.0f;
        _joints.push_back(g_thePhysicsSystem->CreateJoint(spring));
    }

    {
        RodJointDef rod{};
        rod.rigidBodyA = &_bodies[2];
        rod.rigidBodyB = &_bodies[3];
        rod.length = 55.0f;
        _joints.push_back(g_thePhysicsSystem->CreateJoint(rod));
    }

    {
        CableJointDef cable{};
        cable.rigidBodyA = &_bodies[4];
        cable.rigidBodyB = &_bodies[5];
        cable.length = 55.0f;
        _joints.push_back(g_thePhysicsSystem->CreateJoint(cable));
    }
    _activeJoint = _joints[0];

    std::vector<RigidBody*> body_ptrs(_bodies.size());
    for(std::size_t i = 0u; i < _bodies.size(); ++i) {
        body_ptrs[i] = &_bodies[i];
    }
    g_thePhysicsSystem->SetWorldDescription(physicsSystemDesc);
    g_thePhysicsSystem->AddObjects(body_ptrs);

    g_thePhysicsSystem->Enable(true);
    g_thePhysicsSystem->Debug_ShowCollision(true);

}

void GameStateConstraints::OnExit() noexcept {
    g_thePhysicsSystem->RemoveAllObjectsImmediately();
    g_thePhysicsSystem->Debug_ShowCollision(false);
    g_thePhysicsSystem->Enable(false);
    _bodies.clear();
    _joints.clear();
}


void GameStateConstraints::BeginFrame() noexcept {
    /* DO NOTHING */
}

void GameStateConstraints::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::Esc)) {
        g_theApp->SetIsQuitting(true);
        return;
    }
    g_thePhysicsSystem->Debug_ShowWorldPartition(_show_world_partition);
    g_thePhysicsSystem->Debug_ShowCollision(_show_collision);
    g_thePhysicsSystem->Debug_ShowJoints(_show_joints);
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
    _activeBody->ApplyImpulse(direction * 1000.0f);
}

void GameStateConstraints::ShowDebugWindow() {
    if(ImGui::Begin("Debug Window", &_show_debug_window)) {
        ImGui::Checkbox("Show Collision", &_show_collision);
        ImGui::Checkbox("Show Joints", &_show_joints);
        Debug_SelectedBodiesComboBoxUI();
        Debug_ShowBodiesUI();
        Debug_ShowJointsUI();
    }
    ImGui::End();
}

void GameStateConstraints::Debug_SelectedBodiesComboBoxUI() {
    const auto b_size = _bodies.size();
    const auto distance_between_b2b3 = MathUtils::CalcDistance(_bodies[2].GetPosition(), _bodies[3].GetPosition());
    ImGui::Text("B2B3 Distance: %.02f", distance_between_b2b3);
    const auto distance_between_b4b5 = MathUtils::CalcDistance(_bodies[4].GetPosition(), _bodies[5].GetPosition());
    ImGui::Text("B4B5 Distance: %.02f", distance_between_b4b5);
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

void GameStateConstraints::Debug_ShowBodiesUI() {
    const auto b_size = _bodies.size();
    std::string header = std::string{"Bodies - "} + std::to_string(b_size);
    if(ImGui::CollapsingHeader(header.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
        for(std::size_t i = 0; i < b_size; ++i) {
            const auto* body = &_bodies[i];
            std::string bodies_tree_name{"Body "};
            bodies_tree_name += std::to_string(i);
            if(ImGui::TreeNode(reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::hash<std::string>{}(bodies_tree_name))), bodies_tree_name.c_str())) {
                Debug_ShowBodyParametersUI(body);
                ImGui::TreePop();
            }
        }
    }
}

void GameStateConstraints::Debug_ShowJointsUI() {
    const auto& joints = g_thePhysicsSystem->Debug_GetJoints();
    const auto j_size = joints.size();
    std::string joints_header = std::string{"Joints - "} + std::to_string(j_size);
    if(ImGui::CollapsingHeader(joints_header.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
        for(std::size_t i = 0; i < j_size; ++i) {
            auto& joint = *joints[i].get();
            std::string joints_tree_name{"Joint "};
            joints_tree_name += std::to_string(i);
            if(ImGui::TreeNode(reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::hash<std::string>{}(joints_tree_name))), joints_tree_name.c_str())) {
                const auto* const bodyA = joint.GetBodyA();
                const auto* const bodyB = joint.GetBodyB();
                const auto anchorA = joint.GetAnchorA();
                const auto anchorB = joint.GetAnchorB();
                for(std::size_t j = 0; j < 2; ++j) {
                    std::string joints_body_header{"Body "};
                    joints_body_header += static_cast<char>('A' + j);
                    if(ImGui::TreeNode(reinterpret_cast<void*>(static_cast<std::uintptr_t>(std::hash<std::string>{}(joints_body_header))), joints_body_header.c_str())) {
                        _activeJoint = &joint;
                        if(ImGui::Button("Detach")) {
                            joint.Detach(j == 0 ? bodyA : bodyB);
                        }
                        Debug_ShowBodyParametersUI(j == 0 ? bodyA : bodyB);
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        }
    }
}

void GameStateConstraints::Debug_ShowBodyParametersUI(const RigidBody* const body) {
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
