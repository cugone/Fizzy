#pragma once

#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Physics/RigidBody.hpp"

#include "Engine/Math/Vector2.hpp"

#include "Engine/Renderer/Camera2D.hpp"

#include "Game/IState.hpp"

#include <guiddef.h>

class GameStateGravityDrag : public IState {
public:

    // {4A8529AB-0CCE-44A4-B039-6ADEB8D270E0}
    static inline constexpr GUID ID = {0x4a8529ab, 0xcce, 0x44a4, { 0xb0, 0x39, 0x6a, 0xde, 0xb8, 0xd2, 0x70, 0xe0 }};

    GameStateGravityDrag() = default;
    GameStateGravityDrag(const GameStateGravityDrag& other) = default;
    GameStateGravityDrag(GameStateGravityDrag&& other) = default;
    GameStateGravityDrag& operator=(const GameStateGravityDrag& other) = default;
    GameStateGravityDrag& operator=(GameStateGravityDrag&& other) = default;
    virtual ~GameStateGravityDrag() = default;

    void OnEnter() noexcept override;
    void OnExit() noexcept override;

    void BeginFrame() noexcept override;
    void Update([[maybe_unused]] a2de::TimeUtils::FPSeconds deltaSeconds) noexcept override;
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
    
    void Debug_ShowBodiesUI();
    void Debug_ShowBodyParametersUI(const a2de::RigidBody* const body);
    void Debug_SelectedBodiesComboBoxUI();

    std::vector<a2de::RigidBody> _bodies{};
    std::vector<a2de::Vector2> _new_body_positions{};
    a2de::Vector2 _debug_point_on_body{};
    static inline std::size_t _selected_body{0u};
    mutable a2de::Camera2D _ui_camera{};
    a2de::RigidBody* _activeBody{};
    bool _isGravityEnabled = true;
    bool _isDragEnabled = true;
    bool _debug_click_adds_bodies = false;
    bool _show_debug_window = true;
    bool _show_world_partition = true;
    bool _show_collision = true;
};
