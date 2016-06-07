#include "stdafx.h"
#include <AudioInterface.h>
#include <Camera.h>
#include "CreditMenuState.h"
#include <Cursor.h>
#include <ControllerInput.h>
#include <FadeMessage.h>
#include <GUIManager.h>
#include "HatState.h"
#include "InGameState.h"
#include <InputWrapper.h>
#include "MainMenuState.h"
#include <ModelLoader.h>
#include <OnClickMessage.h>
#include "OptionState.h"
#include <PostMaster.h>
#include <PollingStation.h>
#include "StateStackProxy.h"
#include "SplashState.h"
#include <SpriteProxy.h>

MainMenuState::MainMenuState()
	: myMenuAlpha(0.f)
	, myGUIPosition(0.f, 0.f)
	, myGUIEndPosition(0.f, 0.f)
	, myGUIStartPosition(-850.f, 0.f)
	, myGUIManager(nullptr)
	, myHasRunOnce(false)
{
	myWindowSize = CU::Vector2<float>(float(Prism::Engine::GetInstance()->GetWindowSize().x)
		, float(Prism::Engine::GetInstance()->GetWindowSize().y));


	myGUIPosition = myGUIStartPosition;
}

MainMenuState::~MainMenuState()
{
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


	myGUIManager->SetPosition(myGUIPosition);
	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);
	myHasRunOnce = false;
	myCursor->SetShouldRender(true);

	InitControllerInMenu(myController, myGUIManager, myCursor);
	myController->SetIsInMenu(true);
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void MainMenuState::EndState()
{
}

const eStateStatus MainMenuState::Update(const float& aDeltaTime)
{
	if (myHasRunOnce == false)
	{
		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new SplashState("Data/Resource/Texture/Menu/Splash/T_logo_our.dds", false));
		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new SplashState("Data/Resource/Texture/Menu/Splash/T_logo_other.dds", true));
		myHasRunOnce = true;
	}
	else
	{
		//if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true || myController->ButtonOnDown(eXboxButton::BACK))
		//{
		//	myIsActiveState = false;
		//	return eStateStatus::ePopMainState;
		//}

		myMenuAlpha += aDeltaTime;
		if (myGUIPosition.x < 0.f)
		{
			myGUIPosition.x = myTweener.DoTween(myMenuAlpha, myGUIStartPosition.x, myGUIEndPosition.x - myGUIStartPosition.x, 1.f, eTweenType::EXPONENTIAL_HALF);
		}

		HandleControllerInMenu(myController, myGUIManager, myCursor);

		myGUIManager->SetPosition(myGUIPosition);
		myGUIManager->Update(aDeltaTime);
	}
	return myStateStatus;
}

void MainMenuState::Render()
{
	myGUIManager->Render();
}

void MainMenuState::ResumeState()
{
	myIsActiveState = true;
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	myCursor->SetShouldRender(true);
	InitControllerInMenu(myController, myGUIManager, myCursor);
	myController->SetIsInMenu(true);
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void MainMenuState::PauseState()
{
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
	case eOnClickEvent::CREDITS:
		SET_RUNTIME(false);
		myStateStack->PushMainGameState(new CreditMenuState());
		break;
	case eOnClickEvent::OPTIONS:
		SET_RUNTIME(false);
		myStateStack->PushMainGameState(new OptionState());
		break;
	case eOnClickEvent::HAT:
		SET_RUNTIME(false);
		myStateStack->PushMainGameState(new HatState());
		break;
	}
}