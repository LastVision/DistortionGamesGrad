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
#include <fstream>

ScoreState::ScoreState(const CU::GrowingArray<const Score*>& someScores, const ScoreInfo& aScoreInfo, const int aLevelID)
	: myScores(someScores)
	, myScoreInfo(aScoreInfo)
	, myScoreWidgets(4)
{
	for each (const Score* score in myScores)
	{
		myScoreWidgets.Add(new ScoreWidget(score));
	}
	SaveScoreToFile(aLevelID);
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

void ScoreState::SaveScoreToFile(const int aLevelID)
{
	CU::BuildFoldersInPath(CU::GetMyDocumentFolderPath() + "Data/Score/");
	std::fstream file;
	file.open(CU::GetMyDocumentFolderPath() + "Data/Score/Score" + std::to_string(aLevelID).c_str() + ".bin", std::ios::binary | std::ios::in);
	Score currentScore;
	int levelID = 0;
	file >> levelID >> currentScore.myTime;
	file.close();

	file.open(CU::GetMyDocumentFolderPath() + "Data/Score/Score" + std::to_string(aLevelID).c_str() + ".bin", std::ios::binary | std::ios::out);
	if (file.is_open() == true)
	{
		Score highestScore;
		for each (const Score* score in myScores)
		{
			if (highestScore.myTime < score->myTime)
			{
				highestScore.myTime = score->myTime;
				highestScore.myDeathCount = score->myDeathCount;
			}
		}
		if (currentScore.myTime == 0 || highestScore.myTime <= currentScore.myTime)
		{
			file << aLevelID << std::endl << highestScore.myTime << std::endl;
		}
		else
		{
			file << aLevelID << std::endl << currentScore.myTime << std::endl;
		}
	}
}
