
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Win.hpp"

#include "Game/Game.hpp"
#include "Game/GameConfig.hpp"

#include <sstream>
#include <memory>
#include <vector>

#pragma warning(push)
#pragma warning(disable: 28251) // No annotations for int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    UNUSED(hInstance);
    UNUSED(hPrevInstance);
    UNUSED(nCmdShow);

    auto cmdStr = StringUtils::ConvertUnicodeToMultiByte(pCmdLine);

    Engine<Game>::Initialize(g_title_str, cmdStr);
    Engine<Game>::Run();
    Engine<Game>::Shutdown();
}

#pragma warning(pop)
