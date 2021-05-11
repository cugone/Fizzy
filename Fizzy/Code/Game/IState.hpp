#pragma once

#include "Engine/Core/TimeUtils.hpp"

class IState {
public:
    virtual ~IState() = default;

    virtual void OnEnter() noexcept = 0;
    virtual void OnExit() noexcept = 0;
    virtual void BeginFrame() noexcept = 0;
    virtual void Update([[maybe_unused]] a2de::TimeUtils::FPSeconds deltaSeconds) noexcept = 0;
    virtual void Render() const noexcept = 0;
    virtual void EndFrame() noexcept = 0;

protected:
private:
    
};
