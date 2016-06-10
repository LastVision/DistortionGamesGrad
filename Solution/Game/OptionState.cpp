#include "stdafx.h"

#include "AreYouSureState.h"
#include <AudioInterface.h>
#include <ControllerInput.h>
#include <Cursor.h>
#include <FadeMessage.h>
#include <GUIManager.h>
#include <InputWrapper.h>
#include <OnClickMessage.h>
#include "OptionState.h"
#include <PostMaster.h>
#include <Text.h>

OptionState::OptionState()
{
}


OptionState::~OptionState()
{
	SAFE_DELETE(myGUIManager);
	myStateStack = nullptr;
	myCursor = nullptr;
	myController = nullptr;
	PostMaster::GetInstance()->UnSubscribe(this, 0);
}

void OptionState::InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor)
{
	myStateStack = aStateStackProxy;
	myController = aController;
	myCursor = aCursor;
	myStateStatus = eStateStatus::eKeepState;
	myIsActiveState = true;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_options.xml", nullptr, -1);
	myCursor->SetShouldRender(true);
	InitControllerInMenu(myController, myGUIManager, myCursor);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	myController->SetIsInMenu(true);
	myIsLetThrough = true;
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void OptionState::EndState()
{
	myIsActiveState = false;
	myCursor->SetShouldRender(false);
}

const eStateStatus OptionState::Update(const float& aDeltaTime)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true || myController->ButtonOnDown(eXboxButton::BACK)
		|| myController->ButtonOnDown(eXboxButton::B))
	{
		myIsActiveState = false;
		myCursor->SetShouldRender(false);
		return eStateStatus::ePopMainState;
	}

	HandleControllerInMenu(myController, myGUIManager, myCursor);

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void OptionState::Render()
{
	myGUIManager->Render();
}

void OptionState::ResumeState()
{
	myIsActiveState = true;
	myCursor->SetShouldRender(true);
	InitControllerInMenu(myController, myGUIManager, myCursor);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	myController->SetIsInMenu(true);

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void OptionState::PauseState()
{
	PostMaster::GetInstance()->UnSubscribe(this, eMessageType::ON_CLICK);
}

void OptionState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

void OptionState::ReceiveMessage(const OnClickMessage& aMessage)
{
	switch (aMessage.myEvent)
	{
	case eOnClickEvent::GAME_QUIT:
		myStateStatus = eStateStatus::ePopMainState;
		break;
	case eOnClickEvent::OPTIONS_RESET:
		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new AreYouSureState());
		break;
	}
}