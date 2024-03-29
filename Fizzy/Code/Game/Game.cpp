#include "Game/Game.hpp"

#include "Engine/Core/BuildConfig.hpp"
#include "Engine/Core/DataUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/KerningFont.hpp"
#include "Engine/Core/Utilities.hpp"

#include "Engine/Input/InputSystem.hpp"

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector4.hpp"

#include "Engine/Physics/PhysicsUtils.hpp"

#include "Engine/Renderer/AnimatedSprite.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Window.hpp"

#include "Engine/UI/UISystem.hpp"

#include "Game/GameConfig.hpp"

#include "Game/GameStateGravityDrag.hpp"

#include <array>

void Game::Initialize() noexcept {
    g_theRenderer->RegisterMaterialsFromFolder(std::string{ "Data/Materials" });
    _state.ChangeState(GameStateGravityDrag::ID);
}

void Game::BeginFrame() noexcept {
    _state.BeginFrame();
}

void Game::Update(TimeUtils::FPSeconds deltaSeconds) noexcept {
    _state.Update(deltaSeconds);
    if(g_theInputSystem->WasKeyJustPressed(KeyCode::R)) {
        _state.RestartState();
    }
    ShowDemoSelectionWindow();
}

void Game::ShowDemoSelectionWindow() noexcept {
    if(ImGui::Begin("Demo", &_show_debug_window, ImGuiWindowFlags_AlwaysAutoResize)) {
        std::array items{"GravityDrag", "Constraints", "Sleep Management"};
        const char* current_item = items[_demo_index];
        if(ImGui::BeginCombo("Demo", current_item)) {
            for(auto it = std::cbegin(items); it != std::cend(items); ++it) {
                const bool is_selected = current_item == *it;
                if(ImGui::Selectable(*it, is_selected)) {
                    current_item = *it;
                    _demo_index = std::distance(std::cbegin(items), it);
                }
                if(is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
                switch(_demo_index) {
                case 0:
                    _state.ChangeState(GameStateGravityDrag::ID);
                    break;
                case 1:
                    _state.ChangeState(GameStateConstraints::ID);
                    break;
                case 2:
                    _state.ChangeState(GameStateSleepManagement::ID);
                    break;
                default: ERROR_AND_DIE("Game State values have changed. Refactor Demo GUI code.");
                }
            }
            ImGui::EndCombo();
        }
        if(ImGui::Button("Restart Demo")) {
            _state.RestartState();
        }
    }
    ImGui::End();
}

void Game::Render() const noexcept {
    _state.Render();
}

void Game::EndFrame() noexcept {
    _state.EndFrame();
}
