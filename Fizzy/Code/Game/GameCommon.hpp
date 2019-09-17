#pragma once

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/JobSystem.hpp"
#include "Engine/Core/FileLogger.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/Config.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/UI/UISystem.hpp"

#include "Game/App.hpp"
#include "Game/Game.hpp"

extern JobSystem* g_theJobSystem;
extern FileLogger* g_theFileLogger;
extern Renderer* g_theRenderer;
extern Console* g_theConsole;
extern Config* g_theConfig;
extern UISystem* g_theUISystem;
extern InputSystem* g_theInputSystem;
extern AudioSystem* g_theAudioSystem;
extern PhysicsSystem* g_thePhysicsSystem;
extern App* g_theApp;
extern Game* g_theGame;
extern EngineSubsystem* g_theSubsystemHead;
