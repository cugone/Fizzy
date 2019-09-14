#pragma once

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Renderer/Camera2D.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/OBB2.hpp"

struct PhysicsObject {
    OBB2 obb;
    bool is_colliding = false;
};

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

    PhysicsObject _test1{};
    PhysicsObject _test2{};
    PhysicsObject _test3{};
    Disc2 _closest_point{};
    mutable Camera2D _ui_camera{};
    float _cam_speed = 1.0f;
    float _max_shake_angle = 0.0f;
    float _max_shake_x = 0.0f;
    float _max_shake_y = 0.0f;
    float t = 0.5f;
    bool _show_debug_window = false;
};

