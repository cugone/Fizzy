#pragma once

#include "Game/IState.hpp"

#include <guiddef.h>

class GameStateRestartCurrentState : public IState {
public:

    // {DAA2A6B7-FE7E-4911-B352-0022C028C9D3}
    static inline constexpr GUID ID = {0xdaa2a6b7, 0xfe7e, 0x4911, { 0xb3, 0x52, 0x0, 0x22, 0xc0, 0x28, 0xc9, 0xd3 }};

    GameStateRestartCurrentState() = default;
    GameStateRestartCurrentState(const GameStateRestartCurrentState& other) = default;
    GameStateRestartCurrentState(GameStateRestartCurrentState&& other) = default;
    GameStateRestartCurrentState& operator=(const GameStateRestartCurrentState& other) = default;
    GameStateRestartCurrentState& operator=(GameStateRestartCurrentState&& other) = default;
    virtual ~GameStateRestartCurrentState() = default;

    void OnEnter() noexcept override;
    void OnExit() noexcept override;

    void BeginFrame() noexcept override;
    void Update([[maybe_unused]] a2de::TimeUtils::FPSeconds deltaSeconds) noexcept override;
    void Render() const noexcept override;
    void EndFrame() noexcept override;

protected:
private:
};
