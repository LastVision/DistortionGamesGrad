#include "stdafx.h"
#include <Cursor.h>
#include "LevelSelectState.h"
#include <InputWrapper.h>
#include <GUIManager.h>

LevelSelectState::LevelSelectState()
{
}

LevelSelectState::~LevelSelectState()
{
	SAFE_DELETE(myGUIManager);
	myStateStack = nullptr;
	myCursor = nullptr;
}

void LevelSelectState::InitState(StateStackProxy* aStateStackProxy, GUI::Cursor* aCursor)
{
	myCursor = aCursor;
	myStateStack = aStateStackProxy;
	myStateStatus = eStateStatus::eKeepState;
	myIsActiveState = true;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_level_select.xml", nullptr, -1);
	myCursor->SetShouldRender(true);
}

void LevelSelectState::EndState()
{
	myIsActiveState = false;
	myCursor->SetShouldRender(false);
}

const eStateStatus LevelSelectState::Update(const float& aDeltaTime)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true)
	{
		myIsActiveState = false;
		myCursor->SetShouldRender(false);
		return eStateStatus::ePopMainState;
	}

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void LevelSelectState::Render()
{
	myGUIManager->Render();
	//myCursor->Render();
}

void LevelSelectState::ResumeState()
{
	myIsActiveState = true;
	myCursor->SetShouldRender(true);
}

void LevelSelectState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}