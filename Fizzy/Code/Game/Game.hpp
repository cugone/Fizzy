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
    void Update(a2de::TimeUtils::FPSeconds deltaSeconds);
    void Render() const;
    void EndFrame();

protected:
private:
    void ShowDemoSelectionWindow() noexcept;

    GameStateMachine _state{};
    std::vector<a2de::RigidBody> _bodies{};
    std::vector<a2de::Vector2> _new_bodies{};
    static inline std::size_t _demo_index{};
    bool _isGravityEnabled = true;
    bool _isDragEnabled = true;
    bool _show_debug_window = true;
    bool _show_world_partition = true;
    bool _show_collision = true;
};

