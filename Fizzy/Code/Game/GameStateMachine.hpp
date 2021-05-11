#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Game/IState.hpp"
#include "Game/GameStateGravityDrag.hpp"
#include "Game/GameStateConstraints.hpp"
#include "Game/GameStateSleepManagement.hpp"

#include <cstdint>
#include <memory>

#include <guiddef.h>

class GameStateMachine {
public:
    GameStateMachine() = default;
    GameStateMachine(const GUID& initialState);
    GameStateMachine(const GameStateMachine& other) = default;
    GameStateMachine(GameStateMachine&& other) = default;
    GameStateMachine& operator=(const GameStateMachine& other) = default;
    GameStateMachine& operator=(GameStateMachine&& other) = default;
    ~GameStateMachine() = default;

    void ChangeState(const GUID& newStateId) noexcept;
    void RestartState() noexcept;

    void BeginFrame() noexcept;
    void Update([[maybe_unused]] a2de::TimeUtils::FPSeconds deltaSeconds) noexcept;
    void Render() const noexcept;
    void EndFrame() noexcept;

protected:
private:
    bool HasStateChanged() const noexcept;
    void OnExitState() noexcept;
    void OnEnterState(const GUID& enteringStateId) noexcept;

    std::unique_ptr<IState> CreateStateFromId(const GUID& id) noexcept;

    GUID _currentStateId{};
    GUID _nextStateId{};
    std::unique_ptr<IState> _state{};
};
