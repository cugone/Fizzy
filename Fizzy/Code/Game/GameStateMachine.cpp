#include "Game/GameStateMachine.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Game/GameStateGravityDrag.hpp"
#include "Game/GameStateRestartCurrentState.hpp"

bool GameStateMachine::HasStateChanged() const noexcept {
    return !IsEqualGUID(_currentStateId, _nextStateId);
}

GameStateMachine::GameStateMachine(const GUID& initialState)
    : _currentStateId{initialState}
    , _nextStateId{initialState}
{
    
}

void GameStateMachine::ChangeState(const GUID& newStateId) noexcept {
    _nextStateId = newStateId;
}

void GameStateMachine::RestartState() noexcept {
    ChangeState(GameStateRestartCurrentState::ID);
}

void GameStateMachine::OnEnterState(const GUID& enteringStateId) noexcept {
    if(_state = CreateStateFromId(enteringStateId); _state != nullptr) {
        _state->OnEnter();
    } else {
        ERROR_AND_DIE("GameStateMachine::OnEnterState: CreateStateFromId returned an invalid object.");
    }
}

std::unique_ptr<IState> GameStateMachine::CreateStateFromId(const GUID& id) noexcept {
    if(IsEqualGUID(id, GameStateGravityDrag::ID)) {
        return std::make_unique<GameStateGravityDrag>();
    } else if(IsEqualGUID(id, GameStateRestartCurrentState::ID)) {
        _nextStateId = _currentStateId;
        return CreateStateFromId(_currentStateId);
    } else if(IsEqualGUID(id, GameStateConstraints::ID)) {
        return std::make_unique<GameStateConstraints>();
    }
    return {};
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

