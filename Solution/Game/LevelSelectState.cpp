#include "stdafx.h"
#include <ControllerInput.h>
#include <Cursor.h>
#include <InputWrapper.h>
#include "LevelSelectState.h"
#include <GUIManager.h>
#include <OnClickMessage.h>
#include <PostMaster.h>

LevelSelectState::LevelSelectState(CU::ControllerInput* aController)
	: myController(aController)
{
	myControllerUpIsDown = false;
	myControllerDownIsDown = false;
}

LevelSelectState::~LevelSelectState()
{
	SAFE_DELETE(myGUIManager);
	myStateStack = nullptr;
	myCursor = nullptr;
	myController = nullptr;
	PostMaster::GetInstance()->UnSubscribe(this, 0);
}

void LevelSelectState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myCursor = aCursor;
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::eKeepState;
	myIsActiveState = true;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_level_select.xml", nullptr, -1);
	myCursor->SetShouldRender(true);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
}

void LevelSelectState::EndState()
{
	myIsActiveState = false;
	myCursor->SetShouldRender(false);
}

const eStateStatus LevelSelectState::Update(const float& aDeltaTime)
{
	myController->Update(aDeltaTime);
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true)
	{
		myIsActiveState = false;
		myCursor->SetShouldRender(false);
		return eStateStatus::ePopMainState;
	}

	HandleControllerInMenu(myController, myGUIManager);

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void LevelSelectState::Render()
{
	myGUIManager->Render();
}

void LevelSelectState::ResumeState()
{
	myIsActiveState = true;
	myCursor->SetShouldRender(true);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
}

void LevelSelectState::PauseState()
{
	PostMaster::GetInstance()->UnSubscribe(this, eMessageType::ON_CLICK);
}

void LevelSelectState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

void LevelSelectState::ReceiveMessage(const OnClickMessage& aMessage)
{
	switch (aMessage.myEvent)
	{
	case eOnClickEvent::GAME_QUIT:
		myStateStatus = eStateStatus::ePopMainState;
		break;
	}
}