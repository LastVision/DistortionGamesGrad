#include "stdafx.h"

#include <ControllerInput.h>
#include "CreditMenuState.h"
#include <Cursor.h>
#include <FadeMessage.h>
#include <GUIManager.h>
#include <InputWrapper.h>
#include "VictoryState.h"
#include <PostMaster.h>

VictoryState::VictoryState()
	: myGUIManager(nullptr)
{
}

VictoryState::~VictoryState()
{
	SAFE_DELETE(myGUIManager);
	myCursor = nullptr;
}

void VictoryState::InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor)
{
	myIsActiveState = true;
	myIsLetThrough = false;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myController = aController;
	myCursor = aCursor;
	myCursor->SetShouldRender(true);

	if (GC::NightmareMode == false)
	{
		myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_victory_screen.xml", nullptr, -1);
		GC::HasBeenInVictoryScreen = true;
	}
	else
	{
		myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_victory_screen_nightmare.xml", nullptr, -1);
		GC::HasBeenInVictoryScreenNightmare = true;
	}

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);
	myController->SetIsInMenu(true);

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void VictoryState::EndState()
{

}

void VictoryState::ResumeState()
{
	myController->SetIsInMenu(true);
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void VictoryState::PauseState()
{

}

const eStateStatus VictoryState::Update(const float& aDeltaTime)
{
	CU::InputWrapper* input = CU::InputWrapper::GetInstance();
	if (input->KeyDown(DIK_ESCAPE) == true || input->KeyDown(DIK_SPACE) == true || input->KeyDown(DIK_RETURN) == true
		|| input->MouseUp(0) == true || input->MouseUp(1) == true || myController->ButtonOnDown(eXboxButton::A))
	{
		SET_RUNTIME(false);
		if (GC::NightmareMode == false)
		{
			myStateStack->PushSubGameState(new CreditMenuState());
		}
		else
		{
			myStateStatus = eStateStatus::ePopSubState;
		}
	}
	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void VictoryState::Render()
{
	myGUIManager->Render();
}

void VictoryState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}