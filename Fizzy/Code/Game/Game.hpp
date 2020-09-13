#pragma once

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Renderer/Camera2D.hpp"
#include "Engine/Physics/PhysicsSystem.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/OBB2.hpp"

#include "Game/GameStateMachine.hpp"

class Game {
public:
    Game() = default;
    Game(const Game& other) = default;
    Game(Game&& other) = default;
    Game& operator=(const Game& other) = default;
    Game& operator=(Game&& other) = default;
    ~Game() = default;

    void Initialize();
    void BeginFrame();
    void Update(TimeUtils::FPSeconds deltaSeconds);
    void Render() const;
    void EndFrame();

protected:
private:
    GameStateMachine _state{};
    std::vector<RigidBody> _bodies{};
    std::vector<Vector2> _new_bodies{};
    mutable Camera2D _ui_camera{};
    float _cam_speed = 100.0f;
    float _max_shake_angle = 0.0f;
    float _max_shake_x = 0.0f;
    float _max_shake_y = 0.0f;
    bool _isGravityEnabled = true;
    bool _isDragEnabled = true;
    bool _show_debug_window = true;
    bool _show_world_partition = true;
    bool _show_collision = true;
};

