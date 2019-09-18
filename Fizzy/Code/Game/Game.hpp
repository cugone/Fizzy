#pragma once

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Renderer/Camera2D.hpp"
#include "Engine/Physics/PhysicsSystem.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/OBB2.hpp"

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
    void ShowDebugWindow();

    void HandleDebugInput(TimeUtils::FPSeconds deltaSeconds, Camera2D& base_camera);
    void HandleDebugKeyboardInput(TimeUtils::FPSeconds deltaSeconds, Camera2D& base_camera);
    void HandleDebugMouseInput(TimeUtils::FPSeconds deltaSeconds, Camera2D& base_camera);

    void HandlePlayerInput(TimeUtils::FPSeconds deltaSeconds, Camera2D& base_camera);

    RigidBody _test1 = RigidBody{ { PhysicsMaterial{}
    ,Vector2(200.0f, 250.0f)
    ,Vector2::ZERO
    ,Vector2::ZERO
    ,OBB2(Vector2(200.0f, 250.0f), Vector2::ONE * 25.0f, 0.0f)
        } };
    RigidBody _test2 = RigidBody{ { PhysicsMaterial{}
    ,Vector2(400.0f, 250.0f)
    ,Vector2::ZERO
    ,Vector2::ZERO
    ,OBB2(Vector2(400.0f, 250.0f), Vector2::ONE * 25.0f, 0.0f)
        } };
    mutable Camera2D _ui_camera{};
    float _cam_speed = 1.0f;
    float _max_shake_angle = 0.0f;
    float _max_shake_x = 0.0f;
    float _max_shake_y = 0.0f;
    bool _show_debug_window = false;
};

