#include "stdafx.h"

#include <ControllerInput.h>
#include <Cursor.h>
#include <GUIManager.h>
#include "InputWrapper.h"
#include <OnClickMessage.h>
#include <PostMaster.h>
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
		myScoreWidgets.Add(new ScoreWidget(score, myScoreInfo));
	}
}


ScoreState::~ScoreState()
{
	SAFE_DELETE(myGUIManager);
	PostMaster::GetInstance()->UnSubscribe(this, 0);
	myScoreWidgets.DeleteAll();
}

void ScoreState::InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor)
{
	myStateStack = aStateStackProxy;
	myController = aController;
	myCursor = aCursor;
	myCursor->SetShouldRender(true);

	myStateStatus = eStateStatus::eKeepState;
	myIsLetThrough = true;
	myIsActiveState = true;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_score_screen.xml", nullptr, -1);
	myGUIManager->SetSelectedButton(0, 6);

	InitControllerInMenu(myController, myGUIManager);

	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
}

void ScoreState::EndState()
{
	myCursor->SetShouldRender(false);
}

const eStateStatus ScoreState::Update(const float& aDeltaTime)
{
	//if (CU::InputWrapper::GetInstance()->KeyDown(DIK_RETURN) || myController->ButtonOnDown(eXboxButton::A))
	//{
	//	return eStateStatus::ePopMainState;
	//}

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

	for (int i = 0; i < myScoreWidgets.Size(); ++i)
	{
		myScoreWidgets[i]->Render(CU::Vector2<float>(i * 532.f, 0));
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

void ScoreState::ReceiveMessage(const OnClickMessage& aMessage)
{
	switch (aMessage.myEvent)
	{
	case eOnClickEvent::GAME_QUIT:
	case eOnClickEvent::RESTART_LEVEL:
	case eOnClickEvent::NEXT_LEVEL:
		myStateStatus = eStateStatus::ePopMainState;
		break;
	}
}
