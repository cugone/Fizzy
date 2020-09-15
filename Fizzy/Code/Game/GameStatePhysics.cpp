#include "Game/GameStatePhysics.hpp"

#include "Engine/Physics/PhysicsUtils.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

void GameStatePhysics::OnEnter() noexcept {
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
    float x2 = x1;
    float y2 = y1 - 55.0f;
    float x3 = x2 - 55.0f;
    float y3 = y2;

    _bodies.push_back(RigidBody{physicsSystemDesc, RigidBodyDesc{
                    Vector2(x1, y1)
                    ,Vector2::ZERO
                    ,Vector2::ZERO
                    ,std::move(std::make_unique<ColliderCircle>(Vector2(x1, y1), 25.0f))
                    ,PhysicsMaterial{0.0f, 0.0f}
                    ,PhysicsDesc{0.0f}
                    }});
    _bodies.back().EnableGravity(false);
    _bodies.push_back(RigidBody{physicsSystemDesc, RigidBodyDesc{
                    Vector2(x2, y2)
                    ,Vector2::ZERO
                    ,Vector2::ZERO
                    ,std::move(std::make_unique<ColliderCircle>(Vector2(x2, y2), 25.0f))
                    ,PhysicsMaterial{0.0f, 0.0f}
                    ,PhysicsDesc{}
                    }});
    _bodies.push_back(RigidBody{physicsSystemDesc, RigidBodyDesc{
                Vector2(x3, y3)
                ,Vector2::Y_AXIS * 1.0f
                ,Vector2::ZERO
                ,std::move(std::make_unique<ColliderCircle>(Vector2(x3, y3), 25.0f))
                ,PhysicsMaterial{0.0f, 0.0f}
                ,PhysicsDesc{}
                }});
    _bodies.back().EnableGravity(false);
    _bodies.back().EnableDrag(true);
    std::vector<RigidBody*> body_ptrs(_bodies.size());
    for(std::size_t i = 0u; i < _bodies.size(); ++i) {
        body_ptrs[i] = &_bodies[i];
    }
    g_thePhysicsSystem->SetWorldDescription(physicsSystemDesc);
    g_thePhysicsSystem->AddObjects(body_ptrs);
    g_thePhysicsSystem->Enable(true);
    g_thePhysicsSystem->DebugShowCollision(true);
}

void GameStatePhysics::OnExit() noexcept {
    g_thePhysicsSystem->RemoveAllObjectsImmediately();
    g_thePhysicsSystem->DebugShowCollision(false);
    g_thePhysicsSystem->Enable(false);
    _bodies.clear();
}


void GameStatePhysics::BeginFrame() noexcept {
    /* DO NOTHING */
}

void GameStatePhysics::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
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
    base_camera.Update(deltaSeconds);
    HandleInput();
}

void GameStatePhysics::Render() const noexcept {
    g_theRenderer->ResetModelViewProjection();
    g_theRenderer->SetRenderTargetsToBackBuffer();
    g_theRenderer->ClearDepthStencilBuffer();

    g_theRenderer->ClearColor(Rgba::Black);

    g_theRenderer->SetViewportAsPercent();

    //2D View / HUD
    const float ui_view_height = GRAPHICS_OPTION_WINDOW_HEIGHT;
    const float ui_view_width = ui_view_height * _ui_camera.GetAspectRatio();
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
}

void GameStatePhysics::EndFrame() noexcept {
    for(const auto& pos : _new_body_positions) {
        auto new_body = RigidBody{
            g_thePhysicsSystem->GetWorldDescription()
            ,RigidBodyDesc{
                pos
                ,Vector2::ZERO
                ,Vector2::ZERO
                ,std::move(std::make_unique<ColliderCircle>(pos, 25.0f))
                ,PhysicsMaterial {}
                ,PhysicsDesc{}
            }};
        _bodies.push_back(std::move(new_body));
        g_thePhysicsSystem->AddObject(&_bodies.back());
    }
    _new_body_positions.clear();
}

void GameStatePhysics::HandleInput() noexcept {
    HandleKeyboardInput();
    HandleMouseInput();
}

void GameStatePhysics::HandleKeyboardInput() noexcept {
    /* DO NOTHING */
}

void GameStatePhysics::HandleMouseInput() noexcept {
    if(g_theUISystem->WantsInputMouseCapture()) {
        return;
    }
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::LButton)) {
        const auto new_body_position = _ui_camera.WindowToWorldPoint(g_theInputSystem->GetCursorWindowPosition());
        _new_body_positions.push_back(new_body_position);
    }
}

void GameStatePhysics::ShowDebugWindow() {
    if(ImGui::Begin("Debug Window", &_show_debug_window, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Checkbox("Show Quadtree", &_show_world_partition);
        ImGui::Checkbox("Show Collision", &_show_collision);
        if(_bodies.size() > 1) {
            const auto resultGJK = PhysicsUtils::GJK(*_bodies[0].GetCollider(), *_bodies[1].GetCollider());
            const auto resultEPA = PhysicsUtils::EPA(resultGJK, *_bodies[0].GetCollider(), *_bodies[1].GetCollider());
            const auto distance = resultEPA.distance;
            const auto normal = resultEPA.normal;
            static auto sDistance = 0.0f;
            static auto sNormal = Vector3::ZERO;
            if(resultGJK.collides) {
                sDistance = distance;
                sNormal = normal;
            }
            ImGui::Text("GJKDistance: %f", sDistance);
            ImGui::Text("GJKDistance Normal: %f", sNormal);
        } else {
            ImGui::Text("GJKDistance: Invalid");
        }
        if(ImGui::CollapsingHeader("Bodies")) {
            for(std::size_t i = 0; i < _bodies.size(); ++i) {
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