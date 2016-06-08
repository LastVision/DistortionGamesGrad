#include "stdafx.h"

#include <ControllerInput.h>
#include <Cursor.h>
#include <FadeMessage.h>
#include <GUIManager.h>
#include "HatsSelectionState.h"
#include "HelpMenuState.h"
#include <InputWrapper.h>
#include <OnClickMessage.h>
#include "OptionState.h"
#include "PauseMenuState.h"
#include <PostMaster.h>
#include <ReturnToMainMenuMessage.h>
#include <SoundMessage.h>

PauseMenuState::PauseMenuState()
{
}

PauseMenuState::~PauseMenuState()
{
	SAFE_DELETE(myGUIManager);
	myStateStack = nullptr;
	myCursor = nullptr;
	myController = nullptr;
	PostMaster::GetInstance()->UnSubscribe(this, 0);
}


void PauseMenuState::InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor)
{
	myStateStack = aStateStackProxy;
	myController = aController;
	myCursor = aCursor;
	myStateStatus = eStateStatus::eKeepState;
	myIsActiveState = true;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_pause_menu.xml", nullptr, -1);
	myCursor->SetShouldRender(true);
	InitControllerInMenu(myController, myGUIManager, myCursor);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	myIsLetThrough = true;
	myController->SetIsInMenu(true);

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));


	PostMaster::GetInstance()->SendMessage<SoundMessage>(SoundMessage(false));
}

void PauseMenuState::EndState()
{
	myIsActiveState = false;
	myCursor->SetShouldRender(false);

	PostMaster::GetInstance()->SendMessage<SoundMessage>(SoundMessage(true));
}

const eStateStatus PauseMenuState::Update(const float& aDeltaTime)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true || myController->ButtonOnDown(eXboxButton::START)
		|| myController->ButtonOnDown(eXboxButton::B))
	{
		myIsActiveState = false;
		myCursor->SetShouldRender(false);
		return eStateStatus::ePopSubState;
	}

	HandleControllerInMenu(myController, myGUIManager, myCursor);

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void PauseMenuState::Render()
{
	myGUIManager->Render();
}

void PauseMenuState::ResumeState()
{
	myIsActiveState = true;
	myCursor->SetShouldRender(true);
	InitControllerInMenu(myController, myGUIManager, myCursor);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	myController->SetIsInMenu(true);

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void PauseMenuState::PauseState()
{
	PostMaster::GetInstance()->UnSubscribe(this, eMessageType::ON_CLICK);
}

void PauseMenuState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

void PauseMenuState::ReceiveMessage(const OnClickMessage& aMessage)
{
	switch (aMessage.myEvent)
	{
	case eOnClickEvent::HAT_SELECTION:
		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new HatsSelectionState());
		break;
	case eOnClickEvent::GAME_QUIT:
		myStateStatus = eStateStatus::ePopMainState;
		break;
	case eOnClickEvent::RETURN_TO_GAME:
		myStateStatus = eStateStatus::ePopSubState;
		break;
	case eOnClickEvent::OPTIONS:
		SET_RUNTIME(false);
		myStateStack->PushMainGameState(new OptionState());
		break;
	case eOnClickEvent::RETURN_TO_MENU:
		PostMaster::GetInstance()->SendMessage(ReturnToMainMenuMessage());
		myStateStatus = eStateStatus::ePopMainState;
		break;
	case eOnClickEvent::HELP_MENU:
		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new HelpMenuState());
		break;
	}
}