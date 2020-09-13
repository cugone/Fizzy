#include "Game/GameStateMachine.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Game/GameStatePhysics.hpp"
#include "Game/GameStateRestartCurrentState.hpp"

bool GameStateMachine::HasStateChanged() const noexcept {
    return !IsEqualGUID(_currentStateId, _nextStateId);
}

void GameStateMachine::ChangeState(const GUID& newStateId) noexcept {
    _nextStateId = newStateId;
}

void GameStateMachine::RestartState() noexcept {
    ChangeState(GameStateRestartCurrentState::ID);
}

void GameStateMachine::OnEnterState(const GUID& enteringStateId) noexcept {
    if(IsEqualGUID(enteringStateId, GameStatePhysics::ID)) {
        _state = std::make_unique<GameStatePhysics>();
    } else if(IsEqualGUID(enteringStateId, GameStateRestartCurrentState::ID)) {
        OnEnterState(_currentStateId);
        _nextStateId = _currentStateId;
        return;
    } else {
        ERROR_AND_DIE("GameStateMachine::OnEnterState: Invalid state id.");
    }
    _state->OnEnter();
}

void GameStateMachine::OnExitState() noexcept {
    if(_state) {
        _state->OnExit();
        _state.reset(nullptr);
    }
}

void GameStateMachine::BeginFrame() noexcept {
    if(HasStateChanged()) {
        OnExitState();
        OnEnterState(_nextStateId);
        _currentStateId = _nextStateId;
    }
    _state->BeginFrame();
}

void GameStateMachine::Update([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept {
    _state->Update(deltaSeconds);
}

void GameStateMachine::Render() const noexcept {
    _state->Render();
}

void GameStateMachine::EndFrame() noexcept {
    _state->EndFrame();
}

