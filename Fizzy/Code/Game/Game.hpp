#pragma once

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Renderer/Camera2D.hpp"

#include "Engine/Math/AABB2.hpp"
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
    void HandleDebugInput(Camera2D& base_camera);
    void HandleDebugKeyboardInput(Camera2D& base_camera);
    void HandleDebugMouseInput(Camera2D& base_camera);

    void HandlePlayerInput(Camera2D& base_camera);

    AABB2 _test_AABB2{};
    OBB2 _test_OBB2{};
    mutable Camera2D _ui_camera{};
    float _cam_speed = 1.0f;
    float _max_shake_angle = 0.0f;
    float _max_shake_x = 0.0f;
    float _max_shake_y = 0.0f;
    bool _show_debug_window = false;
    bool _do_overlap = false;

};

