#include "stdafx.h"
#include <Cursor.h>
#include "CreditMenuState.h"
#include "PostMaster.h"
#include "GUIManager.h"
#include "InputWrapper.h"
#include "OnClickMessage.h"
#include "InGameState.h"

CreditMenuState::CreditMenuState(float aShowTimer)
	: myGUIManager(nullptr)
	, myShowTimer(aShowTimer)
{
}


CreditMenuState::~CreditMenuState()
{
	SAFE_DELETE(myGUIManager);
	myCursor = nullptr;
	PostMaster::GetInstance()->UnSubscribe(this, eMessageType::ON_CLICK);
}

void CreditMenuState::InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor)
{
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	myIsActiveState = true;
	myIsLetThrough = false;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myCursor = aCursor;
	myController = aController;
	myCursor->SetShouldRender(true);
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_credit_screen.xml", nullptr, -1);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);

	InitControllerInMenu(myController, myGUIManager);
	//PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void CreditMenuState::EndState()
{

}

void CreditMenuState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

const eStateStatus CreditMenuState::Update(const float& aDeltaTime)
{
	myShowTimer -= aDeltaTime;
	if (myShowTimer > 0)
	{
		return myStateStatus;
	}
	CU::InputWrapper* input = CU::InputWrapper::GetInstance();
	if (input->KeyDown(DIK_ESCAPE) == true || input->KeyDown(DIK_SPACE) == true || input->KeyDown(DIK_RETURN) == true
		|| input->MouseUp(0) == true || input->MouseUp(1) == true)
	{
		myStateStatus = eStateStatus::ePopMainState;
	}

	HandleControllerInMenu(myController, myGUIManager);

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void CreditMenuState::Render()
{
	myGUIManager->Render();
}

void CreditMenuState::ResumeState()
{
	InitControllerInMenu(myController, myGUIManager);
	//PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void CreditMenuState::PauseState()
{

}

void CreditMenuState::ReceiveMessage(const OnClickMessage& aMessage)
{
	if (myIsActiveState == true)
	{
		bool notDone = true;
		switch (aMessage.myEvent)
		{
			/*case eOnClickEvent::GAME_START:
			////Prism::MemoryTracker::GetInstance()->SetRunTime(false);
			PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_CLICK, this);
			myStateStack->PushMainGameState(new InGameState(aMessage.myID));
			break;*/

		case eOnClickEvent::GAME_QUIT:
			myStateStatus = eStateStatus::ePopMainState;
			break;
		default:
			DL_ASSERT("Unknown event type.");
			break;
		}
	}
}