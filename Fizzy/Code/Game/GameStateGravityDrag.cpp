#include "Game/GameStateGravityDrag.hpp"

#include "Engine/Core/App.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Input/InputSystem.hpp"

#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Physics/PhysicsTypes.hpp"
#include "Engine/Physics/PhysicsUtils.hpp"
#include "Engine/Physics/SpringJoint.hpp"

#include "Engine/Physics/Particles/ParticleSystem.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Window.hpp"

#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IRendererService.hpp"

#include "Engine/Input/InputSystem.hpp"
#include "Engine/UI/UISystem.hpp"

#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

void GameStateGravityDrag::OnEnter() noexcept {
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
    float radius = 25.0f;
    float x1 = screenX;
    float y1 = screenY;
    float x2 = x1 - 55.0f;
    float y2 = y1;
    float x3 = x1 + 55.0f;
    float y3 = y1;
    float x4 = x3 + 55.0f;
    float y4 = y1;
    float x5 = x4 + 55.0f;
    float y5 = maxs.y + (radius * 4.0f);
    _bodies.push_back(RigidBody(RigidBodyDesc(
        Position{x2, y2}
                    , Velocity{}
                    , Acceleration{}
        , std::make_unique<ColliderCircle>(Position{x2, y2}, radius)
                    , PhysicsMaterial{0.0f, 0.0f}
                    , PhysicsDesc{0.0f}
                    )));
    _bodies.back().EnableGravity(false);
    _bodies.back().EnableDrag(false);
    _bodies.push_back(RigidBody(RigidBodyDesc(
        Position{x1, y1}
                    , Velocity{}
                    , Acceleration{}
                    , std::make_unique<ColliderCircle>(Vector2(x1, y1), radius)
                    ,PhysicsMaterial{0.0f, 0.0f}
                    ,PhysicsDesc{}
                    )));
    _bodies.back().EnableGravity(false);
    _bodies.back().EnableDrag(false);
    _bodies.push_back(RigidBody(RigidBodyDesc(
        Position{x3, y3}
                , Velocity{}
                , Acceleration{}
                , std::make_unique<ColliderCircle>(Vector2(x3, y3), radius)
                , PhysicsMaterial{0.0f, 0.0f}
                , PhysicsDesc{}
                )));
    _bodies.back().EnableGravity(false);
    _bodies.back().EnableDrag(true);
    _bodies.push_back(RigidBody(RigidBodyDesc(
        Position{x4, y4}
                , Velocity{}
                , Acceleration{}
                , std::make_unique<ColliderCircle>(Vector2(x4, y4), radius)
                , PhysicsMaterial{0.0f, 0.0f}
                , PhysicsDesc{}
                )));
    _bodies.back().EnableGravity(true);
    _bodies.back().EnableDrag(true);
    _bodies.push_back(RigidBody(RigidBodyDesc(
        Position{x5, y5}
        , Velocity{}
        , Acceleration{}
        , std::make_unique<ColliderCircle>(Vector2(x5, y5), radius)
        , PhysicsMaterial{0.0f, 0.0f}
        , PhysicsDesc{1.0f}
    )));
    _bodies.back().EnableGravity(false);
    _bodies.back().EnableDrag(false);
    std::vector<RigidBody*> body_ptrs(_bodies.size());
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
    g_thePhysicsSystem->Debug_ShowContacts(true);

    _flamePS = std::make_unique<ParticleSystem>();
    _flamePS->RegisterEffectsFromFolder("Data/ParticleEffects");
    _flame_effect = std::make_unique<ParticleEffect>("flame_emission");
    _flame_effect->position = Vector3{Vector2{x1, y1}, 0.0f};
    _flame_effect->SetPlay(false);
}

void GameStateGravityDrag::OnExit() noexcept {
    _flame_effect->SetPlay(false);
    _flamePS.reset(nullptr);
    g_thePhysicsSystem->RemoveAllObjectsImmediately();
    g_thePhysicsSystem->Debug_ShowCollision(false);
    g_thePhysicsSystem->Debug_ShowContacts(false);
    g_thePhysicsSystem->Enable(false);
    _bodies.clear();
}


void GameStateGravityDrag::BeginFrame() noexcept {
    _flame_effect->BeginFrame();
}

void GameStateGravityDrag::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    auto& renderer = ServiceLocator::get<IRendererService>();
    renderer.UpdateGameTime(deltaSeconds);

    if(g_theInputSystem->WasKeyJustPressed(KeyCode::Esc)) {
        g_theApp<Game>->SetIsQuitting(true);
        return;
    }
    g_thePhysicsSystem->Debug_ShowWorldPartition(_show_world_partition);
    g_thePhysicsSystem->Debug_ShowCollision(_show_collision);
    if(_show_debug_window) {
        ShowDebugWindow();
    }

    Camera2D& base_camera = _ui_camera;
    base_camera.Update(deltaSeconds);

    _debug_point_on_body = MathUtils::CalcClosestPoint(g_theInputSystem->GetMouseCoords(), *_activeBody->GetCollider());
    HandleInput();
    _flame_effect->Update(renderer.GetGameTime().count(), deltaSeconds.count());

}

void GameStateGravityDrag::Render() const noexcept {
    g_theRenderer->BeginRenderToBackbuffer();

    //2D View / HUD
    const auto ui_view_height = static_cast<float>(g_theGame->GetSettings().GetWindowHeight());
    const auto ui_view_width = ui_view_height * _ui_camera.GetAspectRatio();
    const auto ui_view_extents = Vector2{ui_view_width, ui_view_height};
    const auto ui_view_half_extents = ui_view_extents * 0.5f;
    g_theRenderer->BeginHUDRender(_ui_camera, ui_view_half_extents, ui_view_height);

    g_theRenderer->SetMaterial(g_theRenderer->GetMaterial("__2D"));
    if(!g_theInputSystem->IsKeyDown(KeyCode::Shift)) {
        g_theRenderer->DrawFilledCircle2D(_debug_point_on_body, 5.0f);
    } else {
        g_theRenderer->DrawFilledCircle2D(g_theInputSystem->GetMouseCoords(), 5.0f);
    }
    _flame_effect->Render();
    if(_show_debug_window) {
        _flame_effect->DebugRender();
    }
}

void GameStateGravityDrag::EndFrame() noexcept {
    _flame_effect->EndFrame();
    if(_new_body_positions.empty()) {
        return;
    }
    for(const auto& pos : _new_body_positions) {
        _bodies.push_back(RigidBody(RigidBodyDesc(
            pos
            , Vector2::ZERO
            , Vector2::ZERO
            , std::make_unique<ColliderOBB>(pos, Vector2{25.0f,25.0f})
            , PhysicsMaterial{}
            , PhysicsDesc{}
        )));
    }
    const auto new_size = _bodies.size();
    auto new_body_ptrs = std::vector<RigidBody*>(new_size);
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
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::F1)) {
        ToggleShowDebugWindow();
    }
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::F4)) {
        g_theUISystem->ToggleImguiDemoWindow();
    }
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::Enter)) {
        _flame_effect->SetPlay(true);
    }
}

void GameStateGravityDrag::HandleMouseInput() noexcept {
    if(g_theUISystem->WantsInputMouseCapture()) {
        return;
    }
    if(Debug_SelectBody()) {
        return;
    }
    Debug_AddBodyOrApplyForceAtMouseCoords();
}

bool GameStateGravityDrag::Debug_SelectBody() noexcept {
    if(!(g_theInputSystem->WasKeyJustPressed(KeyCode::RButton))) {
        return false;
    }
    const auto s = _bodies.size();
    for(std::size_t i{0u}; i < s; ++i) {
        const auto body = &_bodies[i];
        const auto& p = g_theInputSystem->GetMouseCoords();
        if(MathUtils::IsPointInside(body->GetBounds(), p)) {
            _activeBody = body;
            _selected_body = i;
            return true;
        }
    }
    return false;
}

void GameStateGravityDrag::ToggleShowDebugWindow() noexcept {
    _show_debug_window = !_show_debug_window;
}

void GameStateGravityDrag::Debug_AddBodyOrApplyForceAtMouseCoords() noexcept {
    if(g_theInputSystem->IsKeyDown(KeyCode::Shift) && g_theInputSystem->WasKeyJustPressed(KeyCode::LButton)) {
        Debug_AddBodyAtMouseCoords();
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::LButton)) {
        Debug_ApplyImpulseAtMouseCoords();
    }
}

void GameStateGravityDrag::Debug_AddBodyAtMouseCoords() noexcept {
    const auto& p = g_theInputSystem->GetMouseCoords();
    _new_body_positions.push_back(p);
}

void GameStateGravityDrag::Debug_ApplyImpulseAtMouseCoords() noexcept {
    const auto& p = g_theInputSystem->GetMouseCoords();
    const auto point_on_body = MathUtils::CalcClosestPoint(p, *_activeBody->GetCollider());
    const auto direction = (point_on_body - p).GetNormalize();
    _activeBody->ApplyImpulse(direction * 150.0f);
}

void GameStateGravityDrag::ShowDebugWindow() {
    if(ImGui::Begin("Debug Window", &_show_debug_window)) {
        ImGui::Checkbox("Show Quadtree", &_show_world_partition);
        ImGui::Checkbox("Show Collision", &_show_collision);
        Debug_ShowBodiesUI();
    }
    ImGui::End();
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

void GameStateGravityDrag::Debug_ShowBodyParametersUI(const RigidBody* const body) {
    const auto& acc = body->GetAcceleration();
    const auto& vel = body->GetVelocity();
    const auto& pos = body->GetPosition();
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
