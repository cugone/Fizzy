#pragma once

#include "Engine/Core/TimeUtils.hpp"
#include "Engine/Game/GameBase.hpp"

#include "Engine/Renderer/Camera2D.hpp"
#include "Engine/Physics/PhysicsSystem.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/OBB2.hpp"

#include "Game/GameStateMachine.hpp"

class Game : public GameBase {
public:
    Game() = default;
    Game(const Game& other) = default;
    Game(Game&& other) = default;
    Game& operator=(const Game& other) = default;
    Game& operator=(Game&& other) = default;
    ~Game() = default;

    void Initialize() noexcept override;
    void BeginFrame() noexcept override;
    void Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void Render() const noexcept override;
    void EndFrame() noexcept override;

protected:
private:
    void ShowDemoSelectionWindow() noexcept;

    GameStateMachine _state{};
    std::vector<RigidBody> _bodies{};
    std::vector<Vector2> _new_bodies{};
    static inline std::size_t _demo_index{};
    bool _isGravityEnabled = true;
    bool _isDragEnabled = true;
    bool _show_debug_window = true;
    bool _show_world_partition = true;
    bool _show_collision = true;
};

