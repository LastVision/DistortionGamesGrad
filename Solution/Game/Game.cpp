#include "stdafx.h"

#include <AnimationSystem.h>
#include <AudioInterface.h>
#include <Camera.h>
#include <ColoursForBG.h>
#include <CommonHelper.h>
#include "Console.h"
#include <ControllerInput.h>
#include <Cursor.h>
#include <DebugFont.h>
#include <Engine.h>
#include <EntityFactory.h>
#include <FadeMessage.h>
#include <FileWatcher.h>
#include "Game.h"
#include <HatManager.h>
#include "LevelSelectState.h"
#include <InputWrapper.h>
#include "MainMenuState.h"
#include <ModelLoader.h>
#include <StreakDataContainer.h>
#include <ParticleDataContainer.h>
#include "PollingStation.h"
#include <PostMaster.h>
#include <ResizeMessage.h>
#include "ScriptInterface.h"
#include <ScriptSystem.h>
#include <SystemMonitor.h>
#include <TimerManager.h>
#include <VTuneApi.h>
#include <Vector.h>
#include <XMLReader.h>
#include "InGameState.h"
#include <OnClickMessage.h>

//Hej
Game::Game()
	: myLockMouse(true)
	, myWindowHandler(nullptr)
	, myIsComplete(false)
#ifdef RELEASE_BUILD
	, myShowSystemInfo(false)
#else
	, myShowSystemInfo(true)
#endif
{
	PostMaster::Create();
	Prism::Audio::AudioInterface::CreateInstance();

	Prism::Audio::AudioInterface::GetInstance()->Init("Data/Resource/Sound/Init.bnk");
	Prism::Audio::AudioInterface::GetInstance()->LoadBank("Data/Resource/Sound/GraduationSoundBank.bnk");
	Prism::AnimationSystem::GetInstance();
	Prism::Engine::GetInstance()->SetShowDebugText(myShowSystemInfo);

	myCursor = new GUI::Cursor(Prism::Engine::GetInstance()->GetWindowSizeInt());

	SetCursorPos(Prism::Engine::GetInstance()->GetWindowSizeInt().x / 2, Prism::Engine::GetInstance()->GetWindowSizeInt().y / 2);
	myStateStack.SetCursor(myCursor);

	HatManager::Create();
	HatManager::GetInstance()->LoadHats();
	EntityFactory::GetInstance();
	Prism::ParticleDataContainer::GetInstance();

	std::fstream file;
	file.open("GeneratedData/levelcount.bin", std::ios::binary | std::ios::in);

	DL_ASSERT_EXP(file.is_open() == true, "GeneratedData/levelcount.bin doesn't exist! Run level tool.");

	file >> GC::TotalLevels;

	file.close();
	file.open("GeneratedData/levelcount_nightmare.bin", std::ios::binary | std::ios::in);

	DL_ASSERT_EXP(file.is_open() == true, "GeneratedData/levelcount_nightmare.bin doesn't exist! Run level tool.");
	file >> GC::TotalNightmareLevels;

	file.close();

	file.open(CU::GetMyDocumentFolderPath() + "Data/save.bin", std::ios::binary | std::ios::in);

	if (file.is_open() == true)
	{
		file >> GC::Gold;
	}

	file.close();
}

Game::~Game()
{

	HatManager::Destroy();
	SAFE_DELETE(myTimerManager);
	PostMaster::GetInstance()->UnSubscribe(this, 0);
	SAFE_DELETE(myCursor);
	Prism::Audio::AudioInterface::Destroy();
	Prism::StreakDataContainer::Destroy();
	Prism::ParticleDataContainer::Destroy();
	CU::InputWrapper::Destroy();
	Console::Destroy();
	myStateStack.Clear();
	Prism::DebugDrawer::Destroy();
	EntityFactory::Destroy();
	PostMaster::Destroy();
	//	NetworkManager::Destroy();

	std::fstream file;
	file.open(CU::GetMyDocumentFolderPath() + "Data/save.bin", std::ios::binary | std::ios::out);

	file << GC::Gold;

	file.close();
}


bool Game::Init(HWND& aHwnd)
{
	myWindowHandler = &aHwnd;
	myIsComplete = false;

	PostMaster::GetInstance()->Subscribe(this, eMessageType::FADE | eMessageType::ON_CLICK);

	Prism::Engine::GetInstance()->SetClearColor({ MAGENTA });
	CU::InputWrapper::Create(aHwnd, GetModuleHandle(NULL), DISCL_NONEXCLUSIVE
		| DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	myTimerManager = new CU::TimerManager();


	//Console::GetInstance(); // needed to create console here
	//myStateStack.PushMainGameState(new LevelSelectState(myController));
	//myStateStack.PushMainGameState(new MainMenuState());
#ifdef _DEBUG
	myStateStack.PushMainGameState(new MainMenuState());
#else
	myStateStack.PushMainGameState(new LevelSelectState(false));
#endif

	//PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::LOAD_GAME, 1));
	GAME_LOG("Init Successful");

	myFrameTimes.Init(1024);
	return true;
}

bool Game::Destroy()
{
	return true;
}

bool Game::Update()
{
	CU::InputWrapper::GetInstance()->LogicUpdate();

	myTimerManager->Update();
	Prism::Audio::AudioInterface::GetInstance()->Update();

	float deltaTime = myTimerManager->GetMasterTimer().GetTime().GetFrameTime();
	Prism::Engine::GetInstance()->Update(deltaTime);
	EntityFactory::GetInstance()->UpdateFileWatcher();
	Prism::ParticleDataContainer::GetInstance()->Update();

	float fps = 1.f / deltaTime;
	DEBUG_PRINT(fps);
	float frameTime = deltaTime * 1000;
	DEBUG_PRINT(frameTime);

#ifndef RELEASE_BUILD
	if (myFrameTimes.Size() < 10)
	{
		myFrameTimes.Add(frameTime);
	}

	for (int i = 0; i < myFrameTimes.Size(); ++i)
	{
		//DEBUG_PRINT(myFrameTimes[i]);
	}
	
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_R))
	{
		myFrameTimes.RemoveAll();
	}
#endif

	if (deltaTime > 1.0f / 10.0f)
	{
		deltaTime = 1.0f / 10.0f;
	}

	//if (myLockMouse == true)
	//{
	//	//SetCursorPos(Prism::Engine::GetInstance()->GetWindowSize().x / 2, Prism::Engine::GetInstance()->GetWindowSize().y / 2);
	//	RECT windowRect;
	//	GetWindowRect(*myWindowHandler, &windowRect);
	//	if (Prism::Engine::GetInstance()->IsFullscreen() == false)
	//	{
	//		windowRect.left += 10;
	//		windowRect.top += 35;
	//		windowRect.right -= 10;
	//		windowRect.bottom -= 10;
	//	}
	//	ClipCursor(&windowRect);
	//}

	//if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE))
	//{
	//	return false;
	//}

	if (myStateStack.UpdateCurrentState(deltaTime) == false)
	{
		return false;
	}

	myStateStack.RenderCurrentState();

#ifdef RELEASE_BUILD
	myTimerManager->CapFrameRate(65.f);
#endif
	myCursor->Update();
	myCursor->Render();

	Prism::DebugDrawer::GetInstance()->RenderTextToScreen(); //Have to be last
	return true;
}

void Game::Pause()
{
	myLockMouse = false;
	ShowCursor(true);
}

void Game::UnPause()
{
	myLockMouse = true;
	ShowCursor(false);
}

void Game::OnResize(int aWidth, int aHeight)
{
	myStateStack.OnResize(aWidth, aHeight);
	myCursor->OnResize(aWidth, aHeight);
	//PostMaster::GetInstance()->SendMessage(ResizeMessage(aWidth, aHeight));
}

void Game::ReceiveMessage(const OnClickMessage& aMessage)
{
	switch (aMessage.myEvent)
	{
	case eOnClickEvent::START_LEVEL:
		SET_RUNTIME(false);
		GC::NightmareMode = aMessage.myIsNightmareLevel;
		myStateStack.PushMainGameState(new InGameState(aMessage.myID));
		break;
	case eOnClickEvent::LEVEL_SELECT:
		SET_RUNTIME(false);
		myStateStack.PushMainGameState(new LevelSelectState(aMessage.myIsNightmareLevel));
		break;
	case eOnClickEvent::OPTIONS_TOGGLE_VIBRATION:
		GC::OptionsUseViberations = aMessage.myID != 0;
		break;
	case eOnClickEvent::OPTIONS_TOGGLE_SHADOWS:
		GC::OptionsUseShadows = aMessage.myID != 0;
		break;
	case eOnClickEvent::OPTIONS_TOGGLE_OFFLINE_MODE:
		{
			GC::OptionsEnableOffline = aMessage.myID != 0;
			std::ofstream file(CU::GetMyDocumentFolderPath() + "Data\\Setting\\SET_game_setting.bin", std::ios::binary | std::ios::out);
			if (file.is_open() == true)
			{
				file.write(reinterpret_cast<const char*>(&GC::FirstTimeScoreSubmit), sizeof(bool));
				file.write(reinterpret_cast<const char*>(&GC::OptionsEnableOffline), sizeof(bool));
				file.close();
			}
		}
		break;
	case eOnClickEvent::GAME_QUIT:
		break;
	}
}

void Game::ReceiveMessage(const FadeMessage& aMessage)
{
	Prism::Engine::GetInstance()->StartFade(aMessage.myFadeTime);
}
