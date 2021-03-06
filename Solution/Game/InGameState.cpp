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
	, myLevelFactory(nullptr)
	, myHasStartedMusicBetweenLevels(false)
	, myLastLevel(aLevelID)
	, myNextLevel(-1)
	, myLoadingScreen(nullptr)
	, myIsBetweenLevels(false)
	, myRotationSpeed(3.14f)
	, myRotatingScale(0.f)
	, myScaleTimer(0.f)
	, myState(eState::FIRST_FRAME)
	, myFirstFrameTimer(0.f)
{
	myIsActiveState = false;

	//myCameraOrientation.SetPos(CU::Vector3<float>(0, 10, -7.f));

	myCamera = new Prism::Camera(myCameraOrientation);
	myCamera->Update(1.f / 30.f);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);
	CU::Vector2<float> size = Prism::Engine::GetInstance()->GetWindowSize();

	myLoadingScreen = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/T_loading_screen.dds", size, size * 0.5f);
	myRotatingThing = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/T_loading_screen_rotating_thing.dds", { 256.f, 256.f }, { 128.f, 128.f });
	myBlackSprite = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/T_loading_screen_black.dds", size, size * 0.5f);
}

InGameState::~InGameState()
{
	PostMaster::GetInstance()->UnSubscribe(this, 0);

	Console::Destroy();
	SAFE_DELETE(myCamera);
	SAFE_DELETE(myLevelFactory);
	SAFE_DELETE(myText);
	SAFE_DELETE(myLoadingScreen);
	SAFE_DELETE(myRotatingThing);
	SAFE_DELETE(myBlackSprite);
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

	myIsActiveState = true;

	myText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::CONSOLE));
	myText->SetPosition(CU::Vector2<float>(800.f, 800.f));

	PostMaster::GetInstance()->Subscribe(this, eMessageType::LEVEL_FINISHED | eMessageType::ON_CLICK 
		| eMessageType::RETURN_TO_MENU | eMessageType::RETURN_TO_MAIN_MENU);

	myNextLevel = 1;
	myController->SetIsInMenu(false);


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

	myState = eState::FIRST_FRAME;
	while (Prism::ModelLoader::GetInstance()->IsLoading() == true);

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
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

const eStateStatus InGameState::Update(const float& aDelta)
{
	switch (myState)
	{
	case eState::FIRST_FRAME:
		myFirstFrameTimer += aDelta;
		if (myFirstFrameTimer >= 1.f / 2.5f)
		{
			myState = eState::START_LOADING;
		}
		break;
	case eState::START_LOADING:
		if (myStateStatus != eStateStatus::eKeepState)
		{
			return myStateStatus;
		}

		SET_RUNTIME(false);
		if (myLevelFactory->LoadLevel(myLevel) == true)
		{
			myState = eState::LOADING;
		}
		else
		{
			if ((GC::NightmareMode == false && GC::HasBeenInVictoryScreen == false)
				|| (GC::NightmareMode == true && GC::HasBeenInVictoryScreenNightmare == false))
			{
				myStateStack->PushSubGameState(new VictoryState());
			}
			else
			{
				myStateStatus = eStateStatus::ePopMainState;
			}
		}
	case eState::LOADING:
		if (myScaleTimer <=1.f)
		{
			myScaleTimer += aDelta ;
			myRotatingScale = myTweener.DoTween(myScaleTimer, 0.f, 1.f, 1.f, eTweenType::SINUS_HALF);
		}

		myRotatingThing->Rotate(myRotationSpeed * aDelta);
		if (Prism::ModelLoader::GetInstance()->IsLoading() == false)
		{
			myScaleTimer = 0.f;
			myState = eState::LOADING_FINISHED;
		}
		break;
	case eState::LOADING_FINISHED:		
		myScaleTimer += aDelta;
		myRotatingScale = myTweener.DoTween(myScaleTimer, 1.f, -1.f, 1.f, eTweenType::SINUS_HALF);
		myRotatingThing->Rotate(myRotationSpeed * aDelta);

		if (myScaleTimer > 1.f)
		{
			myState = eState::START_GAME;
		}

		break;
	case eState::START_GAME:
		myStateStack->PushMainGameState(myLevel);
		break;
	}

	return myStateStatus;
}

void InGameState::Render()
{
	CU::Vector2<float> windowSize = Prism::Engine::GetInstance()->GetWindowSize();

	if (myIsBetweenLevels == false)
	{
		myLoadingScreen->Render(windowSize * 0.5f);
		myRotatingThing->Render({ windowSize.x * 0.1f, windowSize.y * 0.75f }, { myRotatingScale, myRotatingScale });
	}
	else if (myState != eState::FIRST_FRAME)
	{
		myBlackSprite->Render(windowSize * 0.5f);
	}
}

void InGameState::ResumeState()
{
	myIsActiveState = true;
	myLevelToLoad = -1;
	myController->SetIsInMenu(false);
	myIsBetweenLevels = true;
	myState = eState::FIRST_FRAME;
	myFirstFrameTimer = 0.f;

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

void InGameState::ReceiveMessage(const ReturnToMainMenuMessage&)
{
	myStateStatus = eStateStatus::ePopMainState;
}