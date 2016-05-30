#include "stdafx.h"

#include <ControllerInput.h>
#include <Cursor.h>
#include <GUIManager.h>
#include "InputWrapper.h"
#include "LevelFactory.h"
#include <OnClickMessage.h>
#include <PostMaster.h>
#include "Score.h"
#include "ScoreInfo.h"
#include "ScoreState.h"
#include "ScoreWidget.h"
#include <WidgetContainer.h>
#include <fstream>

ScoreState::ScoreState(const CU::GrowingArray<const Score*>& someScores, const ScoreInfo& aScoreInfo, const int aLevelID)
	: myScores(someScores)
	, myScoreInfo(aScoreInfo)
	, myScoreWidgets(4)
	, myTimer(2.f)
	, myNumberOfActiveScores(0)
	, myCurrentLevel(aLevelID)
{
	for each (const Score* score in myScores)
	{
		if (score->myActive == true)
		{
			myNumberOfActiveScores++;
		}
		myScoreWidgets.Add(new ScoreWidget(score, myScoreInfo));
	}
	SaveScoreToFile(aLevelID);
	SaveUnlockedLevels(aLevelID);
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

	int nextLevel = myCurrentLevel + 1;

	if (nextLevel < (GC::NightmareMode ? GC::TotalNightmareLevels : GC::TotalLevels))
	{
		static_cast<GUI::WidgetContainer*>(myGUIManager->GetWidgetContainer()->At(0))->At(2)->SetButtonText(std::to_string(nextLevel));
	}

	InitControllerInMenu(myController, myGUIManager, myCursor);

	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	myController->SetIsInMenu(true);

	if (GC::NightmareMode == false && myCurrentLevel >= GC::TotalLevels)
	{
		GC::HasWonGame = true;
	}
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

	myTimer -= aDeltaTime;

	for each (ScoreWidget* widget in myScoreWidgets)
	{
		widget->Update(aDeltaTime);
	}

	if (myTimer < 0)
	{
		HandleControllerInMenu(myController, myGUIManager);

		myGUIManager->Update(aDeltaTime);
	}
	return myStateStatus;
}

void ScoreState::Render()
{
	for each (const Score* score in myScores)
	{
		DEBUG_PRINT(score->myDeathCount);
		DEBUG_PRINT(score->myTime);
	}

	if (myTimer < 0)
	{
		myGUIManager->Render();
	}

	if (myNumberOfActiveScores == 1)
	{
		myScoreWidgets[0]->Render(CU::Vector2<float>((myScoreWidgets[0]->GetSize().x / 2.f), 0.f));
	}
	else 
	{
		for (int i = 0; i < myScoreWidgets.Size(); ++i)
		{
			myScoreWidgets[i]->Render(CU::Vector2<float>(i * 532.f, 0));
		}
	}
}

void ScoreState::ResumeState()
{
	myController->SetIsInMenu(true);
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
	std::string levelsPath = "Data/Score/Score";

	if (GC::NightmareMode == true)
	{
		levelsPath = "Data/Score/Score_Nightmare";
	}

	CU::BuildFoldersInPath(CU::GetMyDocumentFolderPath() + "Data/Score/");
	std::fstream file;
	file.open(CU::GetMyDocumentFolderPath() + levelsPath + std::to_string(aLevelID).c_str() + ".bin", std::ios::binary | std::ios::in);
	Score currentScore;
	int levelID = 0;
	file >> levelID >> currentScore.myTime;
	file.close();

	file.open(CU::GetMyDocumentFolderPath() + levelsPath + std::to_string(aLevelID).c_str() + ".bin", std::ios::binary | std::ios::out);
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

		if (currentScore.myTime != 0 && currentScore.myTime < highestScore.myTime)
		{
			highestScore.myTime = currentScore.myTime;
		}

		int stars = 0;
		if (highestScore.myTime < myScoreInfo.myShortTime && highestScore.myTime > 0)
		{
			stars = 3;
		}
		else if (highestScore.myTime < myScoreInfo.myMediumTime)
		{
			stars = 2;
		}
		else if (highestScore.myTime < myScoreInfo.myLongTime)
		{
			stars = 1;
		}

		file << aLevelID << std::endl << highestScore.myTime << std::endl << stars << std::endl;
	}
	file.close();
}

void ScoreState::SaveUnlockedLevels(const int aLevelID)
{
	std::string levelsPath = "Data/UnlockedLevels.bin";

	if (GC::NightmareMode == true)
	{
		levelsPath = "Data/UnlockedLevels_Nightmare.bin";
	}

	std::fstream file;
	file.open(CU::GetMyDocumentFolderPath() + levelsPath, std::ios::binary | std::ios::in);
	Score currentScore;
	CU::GrowingArray<bool> unlockedLevels(64);
	int levelID = 0;
	bool isUnlocked = false;
	bool isEndOfFile = false;
	while (isEndOfFile == false)
	{
		if (file.eof())
		{
			isEndOfFile = true;
			break;
		}
		file >> levelID >> isUnlocked;
		unlockedLevels.Add(isUnlocked);
	}
	file.close();

	file.open(CU::GetMyDocumentFolderPath() + levelsPath, std::ios::binary | std::ios::out);
	if (file.is_open() == true)
	{
		for (int i = 0; i < unlockedLevels.Size(); ++i)
		{
			if (i + 1 == aLevelID + 1)
			{
				unlockedLevels[i] = true;
			}
			if (i == 0)
			{
				file << i + 1 << std::endl << unlockedLevels[i];
			}
			else
			{
				file << std::endl << i + 1 << std::endl << unlockedLevels[i];
			}
		}
	}
	file.close();
}
