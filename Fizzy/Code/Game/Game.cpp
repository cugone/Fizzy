#include "Game/Game.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/KerningFont.hpp"
#include "Engine/Core/Utilities.hpp"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector4.hpp"

#include "Engine/Physics/PhysicsUtils.hpp"

#include "Engine/Renderer/AnimatedSprite.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Window.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

void Game::Initialize() {
    PROFILE_LOG_SCOPE_FUNCTION();
    g_theRenderer->RegisterMaterialsFromFolder(std::string{ "Data/Materials" });
    ChangeState(-1);
}

void Game::BeginFrame() {
    if(_next_state != _cur_state) {
        if(_next_state < 0) {
            _next_state = _cur_state;
        }
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
        g_thePhysicsSystem->EnableGravity(!_isGravityEnabled);
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::Enter)) {
        const auto window_pos = g_theInputSystem->GetMouseCoords();
        const auto displacement = _bodies[1].GetPosition() - window_pos;
        const auto dir = displacement.GetNormalize();
        const auto magnitude = 10000.0f;
        _bodies[1].ApplyForce(dir * magnitude);
    }
    //if(g_theInputSystem->WasKeyJustPressed(KeyCode::LButton)) {
    //    const auto mouse_pos = g_theInputSystem->GetMouseCoords();
    //    _new_bodies.push_back(mouse_pos);
    //}
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
    float width = static_cast<float>(g_theRenderer->GetOutput()->GetDimensions().x);
    float height = static_cast<float>(g_theRenderer->GetOutput()->GetDimensions().y);
    const std::size_t maxBodies = 2;
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
                    } });
    _bodies.back().EnableGravity(false);
    _bodies.push_back(RigidBody{physicsSystemDesc, RigidBodyDesc{
                    Vector2(x2, y2)
                    ,Vector2::ZERO
                    ,Vector2::ZERO
                    ,std::move(std::make_unique<ColliderCircle>(Vector2(x2, y2), 25.0f))
                    ,PhysicsMaterial{0.0f, 0.0f}
                    ,PhysicsDesc{}
                    } });
    _bodies.push_back(RigidBody{physicsSystemDesc, RigidBodyDesc{
                Vector2(x3, y3)
                ,Vector2::Y_AXIS * 50.0f
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
        _bodies.push_back(RigidBody{ g_thePhysicsSystem->GetWorldDescription(), RigidBodyDesc{
            pos
            ,Vector2::ZERO
            ,Vector2::ZERO
            ,std::move(std::make_unique<ColliderCircle>(pos, 25.0f))
            ,PhysicsMaterial {}
            ,PhysicsDesc{}
            } });
        g_thePhysicsSystem->AddObject(&_bodies.back());
    }
    _new_bodies.clear();
}

void Game::ShowDebugWindow() {
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

void Game::HandleDebugInput(TimeUtils::FPSeconds deltaSeconds, Camera2D& base_camera) {
    HandleDebugKeyboardInput(deltaSeconds, base_camera);
    HandleDebugMouseInput(deltaSeconds, base_camera);
}

void Game::HandleDebugKeyboardInput(TimeUtils::FPSeconds /*deltaSeconds*/, Camera2D& /*base_camera*/) {
    if(g_theUISystem->GetIO().WantCaptureKeyboard) {
        return;
    }
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::F1)) {
        _show_debug_window = !_show_debug_window;
    }
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::F4)) {
        g_theUISystem->ToggleImguiDemoWindow();
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::W)) {
        /* DO NOTHING */
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::A)) {
        /* DO NOTHING */
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::S)) {
        /* DO NOTHING */
    }
    if(g_theInputSystem->IsKeyDown(KeyCode::D)) {
        /* DO NOTHING */
    }
}

void Game::HandleDebugMouseInput(TimeUtils::FPSeconds /*deltaSeconds*/, Camera2D& /*base_camera*/) {
    if(g_theUISystem->GetIO().WantCaptureMouse) {
        return;
    }
}
