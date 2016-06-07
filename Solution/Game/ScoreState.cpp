#include "stdafx.h"

#include <ControllerInput.h>
#include <Cursor.h>
#include <FadeMessage.h>
#include <GUIManager.h>
#include <GameConstants.h>
#include "HatState.h"
#include "InputWrapper.h"
#include "LevelFactory.h"
#include <ModelLoader.h>
#include <OnClickMessage.h>
#include <PostMaster.h>
#include "Score.h"
#include "ScoreInfo.h"
#include "ScoreState.h"
#include "ScoreWidget.h"
#include <SpriteProxy.h>
#include <WidgetContainer.h>
#include <fstream>
#include <SpriteAnimator.h>
#include <SQLWrapper.h>

ScoreState::ScoreState(const CU::GrowingArray<const Score*>& someScores, const ScoreInfo& aScoreInfo, const int aLevelID)
	: myScores(someScores)
	, myScoreInfo(aScoreInfo)
	, myScoreWidgets(4)
	, myTimer(2.f)
	, myNumberOfActiveScores(0)
	, myCurrentLevel(aLevelID)
	//, myGoldBagSprite(nullptr)
	, myEarnedStars(0)
	, myEarnedStarsText("")
	, myGoldCostMovement(0.f)
	, myGoldCostFade(1.f)
	, myAnimationsToRun(0)
{
	if (GC::NightmareMode == true)
	{
		myCurrentLevel += 1000;
	}
	for each (const Score* score in myScores)
	{
		if (score->myActive == true)
		{
			myNumberOfActiveScores++;
		}
		myScoreWidgets.Add(new ScoreWidget(score, myScoreInfo));
	}
	GC::CurrentActivePlayers = myNumberOfActiveScores;
	SaveScoreToFile(aLevelID);
	SaveUnlockedLevels(aLevelID);
	CU::SQLWrapper sql;
	if (GC::OptionsEnableOffline == false)
	{
		sql.Connect("server.danielcarlsson.net", "Test@d148087", "DGames2016", "danielcarlsson_net_db_1", CLIENT_COMPRESS | CLIENT_FOUND_ROWS | CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS);
		Score bestScore;
		bestScore.myActive = false;
		for each(const Score* score in myScores)
		{
			if (score->myActive == true)
			{
				if (bestScore.myActive == false || (bestScore.myTime > score->myTime && score->myReachedGoal == true))
				{
					bestScore = *score;
				}
			}
		}
		if (bestScore.myReachedGoal == true)
		{
			sql.WriteHighscore(CU::GetUsername(), bestScore.myTime, myCurrentLevel);
		}
	}

	//myGoldBagSprite = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/T_gold_bag.dds"
	//	, { 128.f, 128.f }, { 64.f, 64.f });
}

ScoreState::~ScoreState()
{
	SAFE_DELETE(myAnimator);
	SAFE_DELETE(myGUIManager);
	//SAFE_DELETE(myGoldBagSprite);
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
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_score_screen.xml", nullptr, myCurrentLevel);
	myGUIManager->SetSelectedButton(0, 6);

	myAnimator = new Prism::SpriteAnimator("Data/Resource/SpriteAnimation/WinBoltAnimation.xml");

	int nextLevel = myCurrentLevel + 1;

	if (nextLevel < (GC::NightmareMode ? GC::TotalNightmareLevels : GC::TotalLevels))
	{
		static_cast<GUI::WidgetContainer*>(myGUIManager->GetWidgetContainer()->At(0))->At(3)->SetButtonText(std::to_string(nextLevel), { -5.f, -30.f});
	}

	InitControllerInMenu(myController, myGUIManager, myCursor);

	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	myController->SetIsInMenu(true);

	if (GC::NightmareMode == false && myCurrentLevel >= GC::TotalLevels)
	{
		GC::HasWonGame = true;
	}	

	while (Prism::ModelLoader::GetInstance()->IsLoading())
	{
	}

	if (myEarnedStars == 0)
	{
		myAnimator->PauseAnimationAtLastFrame();
	}

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
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
		HandleControllerInMenu(myController, myGUIManager, myCursor);

		if (myAnimator->IsPlayingAnimation() == false)
		{
			if (myAnimationsToRun > 0)
			{
				--myAnimationsToRun;
				if (myAnimationsToRun <= 0)
				{
					myAnimator->PauseAnimation();
					myAnimator->UnPauseAnimation();
					myAnimator->RestartAnimation();
				}
				else
				{
					myAnimator->RestartAnimation();
				}
			}
		}

		myGUIManager->Update(aDeltaTime);
	}

	if (myEarnedStars > 0)
	{
		myGoldCostMovement += 25.f * aDeltaTime;
		myGoldCostFade -= 0.25f * aDeltaTime;
	}

	if (myAnimator != nullptr)
	{
		myAnimator->Update(aDeltaTime);
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
		for (int i = 0; i < myScores.Size(); ++i)
		{
			if (myScores[i]->myActive == true)
			{
				myScoreWidgets[i]->Render(CU::Vector2<float>((myScoreWidgets[i]->GetSize().x / 2.f), -80.f));
				break;
			}
		}
	}
	else 
	{
		for (int i = 0; i < myScoreWidgets.Size(); ++i)
		{
			if (i == 0)
			{
				myScoreWidgets[i]->Render(CU::Vector2<float>(-130.f, -80.f));
			}
			else 
			{
				myScoreWidgets[i]->Render(CU::Vector2<float>(i * 580.f, -80.f));
			}
		}
	}

	CU::Vector2<float> goldPos = Prism::Engine::GetInstance()->GetWindowSize() * 0.8f;

	//myGoldBagSprite->Render(goldPos);
	Prism::Engine::GetInstance()->PrintText(GC::Gold, goldPos, Prism::eTextType::RELEASE_TEXT);

	if (myEarnedStars > 0)
	{
		Prism::Engine::GetInstance()->PrintText(myEarnedStarsText, { goldPos.x, goldPos.y + myGoldCostMovement }
		, Prism::eTextType::RELEASE_TEXT, 1.f, { 1.f, myGoldCostFade, myGoldCostFade, myGoldCostFade });
	}

	if (myAnimator != nullptr)
	{
		myAnimator->Render(goldPos);
	}
}

void ScoreState::ResumeState()
{
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	myController->SetIsInMenu(true);

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void ScoreState::PauseState()
{
	PostMaster::GetInstance()->UnSubscribe(this, eMessageType::ON_CLICK);
}

void ScoreState::OnResize(int, int)
{
}

void ScoreState::ReceiveMessage(const OnClickMessage& aMessage)
{
	switch (aMessage.myEvent)
	{
	case eOnClickEvent::HAT:
		SET_RUNTIME(false);
		myStateStack->PushMainGameState(new HatState());
		break;
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
	int currentStars = 0;
	int levelID = 0;
	int diffStars = 0;
	int newStars = 0;

	if (file.is_open() == true)
	{
		file >> levelID >> currentScore.myTime >> currentStars;
	}
	file.close();

	file.open(CU::GetMyDocumentFolderPath() + levelsPath + std::to_string(aLevelID).c_str() + ".bin", std::ios::binary | std::ios::out);
	if (file.is_open() == true)
	{
		Score highestScore;
		highestScore.myTime = 1000000.f;
		for each (const Score* score in myScores)
		{
			if (score->myReachedGoal == true && highestScore.myTime >= score->myTime)
			{
				highestScore.myTime = score->myTime;
				highestScore.myDeathCount = score->myDeathCount;
			}
		}

		if (currentScore.myTime != 0 && currentScore.myTime < highestScore.myTime)
		{
			highestScore.myTime = currentScore.myTime;
		}

		if (highestScore.myTime < myScoreInfo.myShortTime && highestScore.myTime > 0)
		{
			newStars = 3;
		}
		else if (highestScore.myTime < myScoreInfo.myMediumTime)
		{
			newStars = 2;
		}
		else if (highestScore.myTime < myScoreInfo.myLongTime)
		{
			newStars = 1;
		}

		file << aLevelID << std::endl << highestScore.myTime << std::endl << newStars << std::endl;
	}
	file.close();

	diffStars = newStars - currentStars;
	if (diffStars > 0)
	{
		GC::Gold += diffStars;
	}

	myEarnedStars = diffStars > 0 ? diffStars : 0;
	myEarnedStarsText = "+" + std::to_string(myEarnedStars);

	myAnimationsToRun = myEarnedStars;
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
