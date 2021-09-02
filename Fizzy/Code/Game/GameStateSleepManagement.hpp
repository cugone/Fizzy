#pragma once

#include "Engine/Renderer/Camera2D.hpp"

#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Physics/RigidBody.hpp"

#include "Game/IState.hpp"

#include <guiddef.h>

class GameStateSleepManagement : public IState {
public:

    // {6E5190BB-1EC9-4798-91C0-AAB06E3FBFC3}
    static inline constexpr GUID ID = {0x6e5190bb, 0x1ec9, 0x4798, { 0x91, 0xc0, 0xaa, 0xb0, 0x6e, 0x3f, 0xbf, 0xc3 }};

    GameStateSleepManagement() = default;
    GameStateSleepManagement(const GameStateSleepManagement& other) = default;
    GameStateSleepManagement(GameStateSleepManagement&& other) = default;
    GameStateSleepManagement& operator=(const GameStateSleepManagement& other) = default;
    GameStateSleepManagement& operator=(GameStateSleepManagement&& other) = default;
    virtual ~GameStateSleepManagement() = default;

    void OnEnter() noexcept override;
    void OnExit() noexcept override;
    void BeginFrame() noexcept override;
    void Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void Render() const noexcept override;
    void EndFrame() noexcept override;
protected:
private:
    void ShowDebugWindow();
    void ToggleShowDebugWindow() noexcept;

    void FireProjectile() noexcept;

    std::vector<RigidBody> _bodies{};
    mutable Camera2D _ui_camera{};
    bool _isGravityEnabled = true;
    bool _isDragEnabled = true;
    bool _debug_click_adds_bodies = false;
    bool _show_debug_window = true;
    bool _show_world_partition = false;
    bool _show_collision = true;

};
