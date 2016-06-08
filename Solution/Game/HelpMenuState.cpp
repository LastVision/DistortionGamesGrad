#include "stdafx.h"
#include "HelpMenuState.h"
#include <ControllerInput.h>
#include <Cursor.h>
#include <FadeMessage.h>
#include "InGameState.h"
#include "InputWrapper.h"
#include "GUIManager.h"
#include "OnClickMessage.h"
#include "PostMaster.h"

HelpMenuState::HelpMenuState()
	: myGUIManager(nullptr)
{
}

HelpMenuState::~HelpMenuState()
{
	SAFE_DELETE(myGUIManager);
	myCursor = nullptr;
	PostMaster::GetInstance()->UnSubscribe(this, 0);
}

void HelpMenuState::InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	myIsActiveState = true;
	myIsLetThrough = false;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myCursor = aCursor;
	myController = aController;
	myCursor->SetShouldRender(true);
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_help_menu.xml", nullptr, -1);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);

	InitControllerInMenu(myController, myGUIManager, myCursor);
	myController->SetIsInMenu(true);
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void HelpMenuState::EndState()
{
}

const eStateStatus HelpMenuState::Update(const float& aDeltaTime)
{
	CU::InputWrapper* input = CU::InputWrapper::GetInstance();

	if (input->KeyDown(DIK_ESCAPE) == true || myController->ButtonOnDown(eXboxButton::BACK))
	{
		myStateStatus = eStateStatus::ePopSubState;
	}

	HandleControllerInMenu(myController, myGUIManager, myCursor);

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void HelpMenuState::Render()
{
	myGUIManager->Render();
}

void HelpMenuState::ResumeState()
{
	InitControllerInMenu(myController, myGUIManager, myCursor);
	myController->SetIsInMenu(true);
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void HelpMenuState::PauseState()
{

}

void HelpMenuState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

void HelpMenuState::ReceiveMessage(const OnClickMessage& aMessage)
{
	if (myIsActiveState == true)
	{
		switch (aMessage.myEvent)
		{
		case eOnClickEvent::GAME_QUIT:
			myStateStatus = eStateStatus::ePopSubState;
			break;
		}
	}
}