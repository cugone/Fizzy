#include "Game/App.hpp"

#include "Engine/Audio/AudioSystem.hpp"

#include "Engine/Core/Config.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/FileLogger.hpp"
#include "Engine/Core/JobSystem.hpp"
#include "Engine/Core/KeyValueParser.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Engine/Core/EngineSubsystem.hpp"
#include "Engine/Services/ServiceLocator.hpp"
#include "Engine/Services/IAudioService.hpp"
#include "Engine/Services/IConfigService.hpp"
#include "Engine/Services/IFileLoggerService.hpp"
#include "Engine/Services/IInputService.hpp"
#include "Engine/Services/IJobSystemService.hpp"
#include "Engine/Services/IRendererService.hpp"

#include "Engine/Input/InputSystem.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Window.hpp"

#include "Engine/Ui/UISystem.hpp"

#include "Engine/System/System.hpp"

#include "Game/GameCommon.hpp"
#include "Game/GameConfig.hpp"

#include <algorithm>
#include <condition_variable>
#include <iomanip>

bool CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
EngineMessage GetEngineMessageFromWindowsParams(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

EngineMessage GetEngineMessageFromWindowsParams(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    EngineMessage msg{};
    msg.hWnd = hwnd;
    msg.nativeMessage = uMsg;
    msg.wmMessageCode = EngineSubsystem::GetWindowsSystemMessageFromUintMessage(uMsg);
    msg.wparam = wParam;
    msg.lparam = lParam;
    return msg;
}

bool CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return (g_theSubsystemHead &&
            g_theSubsystemHead->EngineSubsystem::ProcessSystemMessage(
                GetEngineMessageFromWindowsParams(hwnd, uMsg, wParam, lParam)));
}


App::App(const std::string& cmdString)
    : EngineSubsystem()
    , _theConfig{std::make_unique<Config>(KeyValueParser{cmdString + "\n" + FileUtils::ReadStringBufferFromFile(g_options_filepath).value_or(g_options_str)})}
{
    SetupEngineSystemPointers();
    SetupEngineSystemChainOfResponsibility();
    LogSystemDescription();
}

App::~App() {
    g_theSubsystemHead = g_theApp;
}

void App::SetupEngineSystemPointers() {
    ServiceLocator::provide(*static_cast<IConfigService*>(_theConfig.get()));

    _theJobSystem = std::make_unique<JobSystem>(-1, static_cast<std::size_t>(JobType::Max), new std::condition_variable);
    ServiceLocator::provide(*static_cast<IJobSystemService*>(_theJobSystem.get()));

    _theFileLogger = std::make_unique<FileLogger>("game");
    ServiceLocator::provide(*static_cast<IFileLoggerService*>(_theFileLogger.get()));

    _theRenderer = std::make_unique<Renderer>();
    ServiceLocator::provide(*static_cast<IRendererService*>(_theRenderer.get()));

    _theInputSystem = std::make_unique<InputSystem>();
    ServiceLocator::provide(*static_cast<IInputService*>(_theInputSystem.get()));

    _theAudioSystem = std::make_unique<AudioSystem>();
    ServiceLocator::provide(*static_cast<IAudioService*>(_theAudioSystem.get()));

    _theUI = std::make_unique<UISystem>();
    _theConsole = std::make_unique<Console>();
    _theGame = std::make_unique<Game>();
    _thePhysicsSystem = std::make_unique<PhysicsSystem>();

    g_theJobSystem = _theJobSystem.get();
    g_theFileLogger = _theFileLogger.get();
    g_theConfig = _theConfig.get();
    g_theRenderer = _theRenderer.get();
    g_theUISystem = _theUI.get();
    g_theConsole = _theConsole.get();
    g_theInputSystem = _theInputSystem.get();
    g_thePhysicsSystem = _thePhysicsSystem.get();
    g_theAudioSystem = _theAudioSystem.get();
    g_theGame = _theGame.get();
    g_theApp = this;
}
void App::SetupEngineSystemChainOfResponsibility() {
    g_theRenderer->SetNextHandler(g_theConsole);
    g_theConsole->SetNextHandler(g_theUISystem);
    g_theUISystem->SetNextHandler(g_theInputSystem);
    g_theInputSystem->SetNextHandler(g_theApp);
    g_theApp->SetNextHandler(nullptr);
    g_theSubsystemHead = g_theRenderer;
}
void App::Initialize() {
    g_theRenderer->Initialize();
    g_theRenderer->SetVSync(currentGraphicsOptions.vsync);
    g_theRenderer->GetOutput()->GetWindow()->custom_message_handler = WindowProc;

    g_theUISystem->Initialize();
    g_theInputSystem->Initialize();
    g_theConsole->Initialize();
    g_theAudioSystem->Initialize();
    g_thePhysicsSystem->Initialize();
    g_theGame->Initialize();
}

void App::BeginFrame() {
    g_theJobSystem->BeginFrame();
    g_theUISystem->BeginFrame();
    g_thePhysicsSystem->BeginFrame();
    g_theInputSystem->BeginFrame();
    g_theConsole->BeginFrame();
    g_theAudioSystem->BeginFrame();
    g_theGame->BeginFrame();
    g_theRenderer->BeginFrame();
}

void App::Update(TimeUtils::FPSeconds deltaSeconds) {
    g_theUISystem->Update(deltaSeconds);
    g_theInputSystem->Update(deltaSeconds);
    g_theConsole->Update(deltaSeconds);
    g_theAudioSystem->Update(deltaSeconds);
    g_thePhysicsSystem->Update(deltaSeconds);
    g_theGame->Update(deltaSeconds);
    g_theRenderer->Update(deltaSeconds);
}

void App::Render() const {
    g_theGame->Render();
    g_thePhysicsSystem->Render();
    g_theUISystem->Render();
    g_theConsole->Render();
    g_theAudioSystem->Render();
    g_theInputSystem->Render();
    g_theRenderer->Render();
}

void App::EndFrame() {
    g_theUISystem->EndFrame();
    g_thePhysicsSystem->EndFrame();
    g_theGame->EndFrame();
    g_theConsole->EndFrame();
    g_theAudioSystem->EndFrame();
    g_theInputSystem->EndFrame();
    g_theRenderer->EndFrame();
}

bool App::ProcessSystemMessage(const EngineMessage& msg) noexcept {

    WPARAM wp = msg.wparam;
    switch(msg.wmMessageCode) {
    case WindowsSystemMessage::Window_Close:
    {
        SetIsQuitting(true);
        return true;
    }
    case WindowsSystemMessage::Window_Quit:
    {
        SetIsQuitting(true);
        return true;
    }
    case WindowsSystemMessage::Window_Destroy:
    {
        ::PostQuitMessage(0);
        return true;
    }
    case WindowsSystemMessage::Window_Size:
    {
        LPARAM lp = msg.lparam;
        if(auto type = EngineSubsystem::GetResizeTypeFromWmSize(msg); type != WindowResizeType::Minimized) {
            const auto w = LOWORD(lp);
            const auto h = HIWORD(lp);

            currentGraphicsOptions.WindowWidth = static_cast<float>(w);
            currentGraphicsOptions.WindowHeight = static_cast<float>(h);
            currentGraphicsOptions.WindowAspectRatio = currentGraphicsOptions.WindowWidth / currentGraphicsOptions.WindowHeight;
        }
        return true;
    }
    case WindowsSystemMessage::Window_ActivateApp:
    {
        bool losing_focus = wp == FALSE;
        bool gaining_focus = wp == TRUE;
        if(losing_focus) {
            _current_focus = false;
            _previous_focus = true;
        }
        if(gaining_focus) {
            _current_focus = true;
            _previous_focus = false;
        }
        return true;
    }
    case WindowsSystemMessage::Keyboard_Activate:
    {
        auto active_type = LOWORD(wp);
        switch(active_type) {
        case WA_ACTIVE: /* FALLTHROUGH */
        case WA_CLICKACTIVE:
            _current_focus = true;
            _previous_focus = false;
            return true;
        case WA_INACTIVE:
            _current_focus = false;
            _previous_focus = true;
            return true;
        default:
            return false;
        }
    }
    default:
        return false;
    }
}

bool App::IsQuitting() const {
    return _isQuitting;
}

void App::SetIsQuitting(bool value) {
    _isQuitting = value;
}

void App::RunFrame() {
    using namespace TimeUtils;

    BeginFrame();

    static FPSeconds previousFrameTime = TimeUtils::GetCurrentTimeElapsed();
    FPSeconds currentFrameTime = TimeUtils::GetCurrentTimeElapsed();
    FPSeconds deltaSeconds = (currentFrameTime - previousFrameTime);
    previousFrameTime = currentFrameTime;

#if _DEBUG
    deltaSeconds = FPSeconds{ std::clamp(FPFrames{ deltaSeconds }, FPFrames{ 0 }, FPFrames{ 1 }) };
#endif

    Update(deltaSeconds);
    Render();
    EndFrame();
}

void App::LogSystemDescription() const {
    auto system = System::GetSystemDesc();
    std::ostringstream ss;
    ss << std::right << std::setfill('-') << std::setw(60) << '\n';
    ss << StringUtils::to_string(system);
    ss << std::right << std::setfill('-') << std::setw(60) << '\n';
    DebuggerPrintf(ss.str().c_str());
}

bool App::HasFocus() const {
    return _current_focus;
}

bool App::LostFocus() const {
    return _previous_focus && !_current_focus;
}

bool App::GainedFocus() const {
    return !_previous_focus && _current_focus;
}
