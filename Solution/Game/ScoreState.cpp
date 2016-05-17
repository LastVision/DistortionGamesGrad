#include "stdafx.h"

#include <ControllerInput.h>
#include <GUIManager.h>
#include "InputWrapper.h"
#include "Score.h"
#include "ScoreInfo.h"
#include "ScoreState.h"
#include "ScoreWidget.h"

ScoreState::ScoreState(const CU::GrowingArray<const Score*>& someScores, const ScoreInfo& aScoreInfo)
	: myScores(someScores)
	, myScoreInfo(aScoreInfo)
	, myScoreWidgets(4)
{
	for each (const Score* score in myScores)
	{
		myScoreWidgets.Add(new ScoreWidget(score));
	}
}


ScoreState::~ScoreState()
{
	SAFE_DELETE(myGUIManager);
	myScoreWidgets.DeleteAll();
}

void ScoreState::InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor)
{
	myStateStack = aStateStackProxy;
	myController = aController;
	myCursor = aCursor;

	myStateStatus = eStateStatus::eKeepState;
	myIsLetThrough = true;
	myIsActiveState = true;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_score_screen.xml", nullptr, -1);

	InitControllerInMenu(myController, myGUIManager);
}

void ScoreState::EndState()
{
}

const eStateStatus ScoreState::Update(const float& aDeltaTime)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_RETURN) || myController->ButtonOnDown(eXboxButton::A))
	{
		return eStateStatus::ePopMainState;
	}

	for each (ScoreWidget* widget in myScoreWidgets)
	{
		widget->Update(aDeltaTime);
	}

	HandleControllerInMenu(myController, myGUIManager);

	myGUIManager->Update(aDeltaTime);
	return myStateStatus;
}

void ScoreState::Render()
{
	for each (const Score* score in myScores)
	{
		DEBUG_PRINT(score->myDeathCount);
		DEBUG_PRINT(score->myTime);
	}

	for each (ScoreWidget* widget in myScoreWidgets)
	{
		widget->Render(CU::Vector2<float>());
	}

	myGUIManager->Render();
}

void ScoreState::ResumeState()
{
}

void ScoreState::PauseState()
{
}

void ScoreState::OnResize(int, int)
{
}
