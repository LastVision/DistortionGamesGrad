#include "stdafx.h"
#include <AudioInterface.h>
#include <Camera.h>
#include <Cursor.h>
#include <ControllerInput.h>
#include <GUIManager.h>
#include "InGameState.h"
#include <InputWrapper.h>
#include "MainMenuState.h"
#include <ModelLoader.h>
#include <OnClickMessage.h>
#include <PostMaster.h>
#include <PollingStation.h>
#include "StateStackProxy.h"
#include "SplashState.h"
#include <SpriteProxy.h>

MainMenuState::MainMenuState()
	: myLogoPosition(0.f, 0.f)
	, myLogoAlpha(0.f)
	, myMenuAlpha(0.f)
	, myDustAlpha(0.f)
	, myGUIPosition(0.f, 0.f)
	, myGUIEndPosition(0.f, 0.f)
	, myGUIStartPosition(-512.f, 0.f)
	, myLogoDone(false)
	, myGUIManager(nullptr)
	, myHasRunOnce(false)
{
	CU::Vector2<float> logoSize(1024.f, 256.f);
	myLogo = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/MainMenu/T_gamelogo.dds"
		, logoSize, logoSize * 0.5f);
	myLogoDust = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/MainMenu/T_gamelogo_dust.dds"
		, logoSize, logoSize * 0.5f);

	myWindowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x)
		, float(Prism::Engine::GetInstance()->GetWindowSize().y));

	myLogoPosition.x = myWindowSize.x * 0.5f;
	myLogoPosition.y = myWindowSize.y + (logoSize.y * 2.f);
	myLogoStartPosition = myLogoPosition;
	myLogoEndPosition.x = myLogoPosition.x;
	myLogoEndPosition.y = myWindowSize.y - (myLogo->GetSize().y * 0.5f);

	myGUIPosition = myGUIStartPosition;
}

MainMenuState::~MainMenuState()
{
	SAFE_DELETE(myLogo);
	SAFE_DELETE(myLogoDust);
	SAFE_DELETE(myGUIManager);
	myCursor = nullptr;
	myController = nullptr;
	PostMaster::GetInstance()->UnSubscribe(this, 0);
}

void MainMenuState::InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	myIsActiveState = true;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myController = aController;
	myCursor = aCursor;
		myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_main_menu.xml", nullptr, -1);


	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_MainMenu", 0);
	myGUIManager->SetPosition(myGUIPosition);
	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);
	myHasRunOnce = false;
	myCursor->SetShouldRender(true);
}

void MainMenuState::EndState()
{
}

const eStateStatus MainMenuState::Update(const float& aDeltaTime)
{
	if (myHasRunOnce == false)
	{
		PostMaster::GetInstance()->UnSubscribe(this, eMessageType::ON_CLICK);
		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new SplashState("Data/Resource/Texture/Menu/Splash/T_logo_our.dds", false));
		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new SplashState("Data/Resource/Texture/Menu/Splash/T_logo_other.dds", true));
		myHasRunOnce = true;
	}
	else 
	{
		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true)
		{
			myIsActiveState = false;
			return eStateStatus::ePopMainState;
		}
		myLogoAlpha += aDeltaTime;
		if (myLogoPosition.y - 25.f > myLogoEndPosition.y)
		{
			myLogoPosition.y = myTweener.DoTween(myLogoAlpha, myLogoStartPosition.y, myLogoEndPosition.y - myLogoStartPosition.y, 1.5f, eTweenType::EXPONENTIAL_HALF);
		}
		else
		{
			myLogoPosition.y = myLogoEndPosition.y + 25.f;
			myMenuAlpha += aDeltaTime;
			if (myLogoDone == false)
			{
				myLogoDone = true;
				myMenuAlpha = 0.f;
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Logo_Thud", 0);
			}
			if (myGUIPosition.x < 0.f)
			{
				myGUIPosition.x = myTweener.DoTween(myMenuAlpha, myGUIStartPosition.x, myGUIEndPosition.x - myGUIStartPosition.x, 1.5f, eTweenType::EXPONENTIAL_HALF);
			}
		}

		myDustAlpha = fmaxf(myLogoAlpha - 2.5f, 0.f);
		myDustAlpha *= 3.f;
		myDustAlpha = fminf(myDustAlpha, 1.f);

		HandleControllerInMenu(myController, myGUIManager);

		myGUIManager->SetPosition(myGUIPosition);
		myGUIManager->Update(aDeltaTime);
	}
	return myStateStatus;
}

void MainMenuState::Render()
{
	myGUIManager->Render();
	//myLogo->Render(myLogoPosition);
	//myLogoDust->Render({ myLogoEndPosition.x, myLogoEndPosition.y + 25.f }, CU::Vector2<float>(0.95f + myDustAlpha * 0.05f, 0.95f + myDustAlpha * 0.05f), CU::Vector4<float>(1.f, 1.f, 1.f, myDustAlpha));
}

void MainMenuState::ResumeState()
{
	myIsActiveState = true;
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	myCursor->SetShouldRender(true);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_MainMenu", 0);
}

void MainMenuState::PauseState()
{
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_MainMenu", 0);
	PostMaster::GetInstance()->UnSubscribe(this, eMessageType::ON_CLICK);
}

void MainMenuState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

void MainMenuState::ReceiveMessage(const OnClickMessage& aMessage)
{
	switch (aMessage.myEvent)
{
	case eOnClickEvent::GAME_QUIT:
		myStateStatus = eStateStatus::ePopMainState;
		break;
	}
}