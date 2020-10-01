#pragma once

#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Physics/RigidBody.hpp"

#include "Engine/Math/Vector2.hpp"

#include "Engine/Renderer/Camera2D.hpp"

#include "Game/IState.hpp"

#include <guiddef.h>

class GameStateConstraints : public IState {
public:

    // {0DB89B96-7391-4134-B78F-6B03D9A07626}
    static inline constexpr GUID ID = {0xdb89b96, 0x7391, 0x4134, { 0xb7, 0x8f, 0x6b, 0x3, 0xd9, 0xa0, 0x76, 0x26 }};

    GameStateConstraints() = default;
    GameStateConstraints(const GameStateConstraints& other) = default;
    GameStateConstraints(GameStateConstraints&& other) = default;
    GameStateConstraints& operator=(const GameStateConstraints& other) = default;
    GameStateConstraints& operator=(GameStateConstraints&& other) = default;
    virtual ~GameStateConstraints() = default;

    void OnEnter() noexcept override;
    void OnExit() noexcept override;

    void BeginFrame() noexcept override;
    void Update([[maybe_unused]]TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void Render() const noexcept override;
    void EndFrame() noexcept override;

    void HandleInput() noexcept;

protected:
private:
    void HandleKeyboardInput() noexcept;
    void HandleMouseInput() noexcept;

    void ShowDebugWindow();
    void ToggleShowDebugWindow() noexcept;

    void Debug_AddBodyOrApplyForceAtMouseCoords() noexcept;
    void Debug_AddBodyAtMouseCoords() noexcept;
    void Debug_ApplyImpulseAtMouseCoords() noexcept;

    void Debug_SelectedBodiesComboBoxUI();
    void Debug_ShowJointsUI();
    void Debug_ShowBodiesUI();
    void Debug_ShowBodyParametersUI(const RigidBody* const body);

    std::vector<RigidBody> _bodies{};
    std::vector<Joint*> _joints{};
    std::vector<Vector2> _new_body_positions{};
    Vector2 _debug_point_on_body{};
    mutable Camera2D _ui_camera{};
    RigidBody* _activeBody{};
    Joint* _activeJoint{};
    bool _isGravityEnabled = true;
    bool _isDragEnabled = true;
    bool _debug_click_adds_bodies = false;
    bool _show_debug_window = true;
    bool _show_world_partition = true;
    bool _show_collision = true;
    bool _show_joints = true;
    static inline std::size_t  _selected_body = 0u;
};
