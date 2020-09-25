#include "Game/Game.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/KerningFont.hpp"
#include "Engine/Core/Utilities.hpp"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector4.hpp"

#include "Engine/Physics/PhysicsUtils.hpp"

#include "Engine/Renderer/AnimatedSprite.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Window.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

#include "Game/GameStateGravityDrag.hpp"

void Game::Initialize() {
    PROFILE_LOG_SCOPE_FUNCTION();
    g_theRenderer->RegisterMaterialsFromFolder(std::string{ "Data/Materials" });
    _state.ChangeState(GameStateGravityDrag::ID);
}

void Game::BeginFrame() {
    _state.BeginFrame();
}

void Game::Update(TimeUtils::FPSeconds deltaSeconds) {
    _state.Update(deltaSeconds);
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::R)) {
        _state.RestartState();
    }
}

void Game::Render() const {
    _state.Render();
}

void Game::EndFrame() {
    _state.EndFrame();
}
