#include "stdafx.h"

#include <AudioInterface.h>
#include "Level.h"
#include "LevelFactory.h"
#include <Camera.h>
#include "Console.h"
#include <ControllerInput.h>
#include <CommonHelper.h>
#include <FadeMessage.h>
#include <GameStateMessage.h>
#include <EffectContainer.h>
#include <EntityFactory.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include <ModelLoader.h>
#include <MurmurHash3.h>
#include <OnClickMessage.h>
#include <PostMaster.h>
#include <ScriptSystem.h>
#include <VTuneApi.h>
#include <TextProxy.h>
#include <SpriteProxy.h>
#include <Cursor.h>
#include <FinishLevelMessage.h>

#include "VictoryState.h"

InGameState::InGameState(int aLevelID)
	: myGUIManager(nullptr)
	, myLevelToLoad(aLevelID)
	, myState(eInGameState::LEVEL)
	, myLevelFactory(nullptr)
	, myFailedLevelHash(false)
	, myHasStartedMusicBetweenLevels(false)
	, myLastLevel(aLevelID)
	, myNextLevel(-1)
	, myIsFirstFrame(true)
	, myLoadingScreen(nullptr)
{
	myIsActiveState = false;

	//myCameraOrientation.SetPos(CU::Vector3<float>(0, 10, -7.f));

	myCamera = new Prism::Camera(myCameraOrientation);
	myCamera->Update(1.f / 30.f);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);
	CU::Vector2<float> size = Prism::Engine::GetInstance()->GetWindowSize();

	myLoadingScreen = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/T_loading_screen.dds", size, size * 0.5f);
}

InGameState::~InGameState()
{
	PostMaster::GetInstance()->UnSubscribe(this, 0);

	Console::Destroy();
	SAFE_DELETE(myCamera);
	SAFE_DELETE(myLevelFactory);
	SAFE_DELETE(myText);
	SAFE_DELETE(myLoadingScreen);
}

void InGameState::InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor)
{
	myIsLetThrough = false;
	myStateStack = aStateStackProxy;
	myController = aController;
	myStateStatus = eStateStatus::eKeepState;
	myCursor = aCursor;
	myCursor->SetShouldRender(false);

	EntityFactory::GetInstance()->LoadEntities("GeneratedData/LI_entity.xml");
	myLevelFactory = new LevelFactory("GeneratedData/LI_level.xml", "GeneratedData/LI_level_nightmare.xml", *myCamera, myLevelToLoad);
	//myLevel = myLevelFactory->LoadLevel(1);


	myIsActiveState = true;

	myText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::CONSOLE));
	myText->SetPosition(CU::Vector2<float>(800.f, 800.f));

	PostMaster::GetInstance()->Subscribe(this, eMessageType::LEVEL_FINISHED | eMessageType::ON_CLICK | eMessageType::RETURN_TO_MENU);

	myNextLevel = 1;
	myController->SetIsInMenu(false);

	//PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));


	if (GC::NightmareMode == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_NightmareInGame", 0);
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_NightmareMenu", 0);
	}
	else
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_InGameMusic", 0);
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_MainMenu", 0);
	}

}

void InGameState::EndState()
{

	if (GC::NightmareMode == true)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_NightmareInGame", 0);
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_NightmareMenu", 0);
	}
	else
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_InGameMusic", 0);
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_MainMenu", 0);
	}

}

const eStateStatus InGameState::Update(const float&)
{
	if (myIsFirstFrame == false)
	{
		if (myStateStatus != eStateStatus::eKeepState)
		{
			return myStateStatus;
		}

		SET_RUNTIME(false);
		Level* level = nullptr;
		if (myLevelFactory->LoadLevel(level) == true)
		{
			myStateStack->PushMainGameState(level);
		}
		else
		{
			if (GC::NightmareMode == false && GC::HasBeenInVictoryScreen == false
				|| GC::NightmareMode == true && GC::HasBeenInVictoryScreenNightmare == false)
			{
				myStateStack->PushSubGameState(new VictoryState());
			}
			else
			{
				myStateStatus = eStateStatus::ePopMainState;
			}
		}
	}
	else
	{
		myIsFirstFrame = false;
	}

	return myStateStatus;
}

void InGameState::Render()
{
	myLoadingScreen->Render(Prism::Engine::GetInstance()->GetWindowSize() * 0.5f);
}

void InGameState::ResumeState()
{
	myIsActiveState = true;
	myLevelToLoad = -1;
	myController->SetIsInMenu(false);
	myIsFirstFrame = true;

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void InGameState::PauseState()
{
}

void InGameState::ReceiveMessage(const FinishLevelMessage& aMessage)
{
	myNextLevel = aMessage.myLevelID;
}

void InGameState::OnResize(int aWidth, int aHeight)
{
	aWidth;
	aHeight;
}

void InGameState::ReceiveMessage(const OnClickMessage& aMessage)
{
	switch (aMessage.myEvent)
	{
	case eOnClickEvent::GAME_QUIT:
		myStateStatus = eStateStatus::ePopMainState;
		break;
	case eOnClickEvent::RESTART_LEVEL:
		SET_RUNTIME(false);
		myLevelFactory->RestartLevel();
		break;
	}
}

void InGameState::ReceiveMessage(const ReturnToMenuMessage&)
{
	myStateStatus = eStateStatus::ePopMainState;
}