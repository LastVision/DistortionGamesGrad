#include "stdafx.h"

#include <ControllerInput.h>
#include <Cursor.h>
#include "FirstTimeFinishLevelState.h"
#include <GrowingArray.h>
#include <GUIManager.h>
#include <InputWrapper.h>
#include <OnClickMessage.h>
#include <PostMaster.h>
#include "ScoreState.h"
#include <fstream>

FirstTimeFinishLevelState::FirstTimeFinishLevelState(const CU::GrowingArray<const Score*>& someScores, const ScoreInfo& aScoreInfo, const int aLevelID)
	: myScores(someScores)
	, myScoreInfo(aScoreInfo)
	, myLevelID(aLevelID)
	, myRenderFlag(true)
{
}


FirstTimeFinishLevelState::~FirstTimeFinishLevelState()
{
	SAFE_DELETE(myGUIManager);
	myStateStack = nullptr;
	myCursor = nullptr;
	myController = nullptr;
	PostMaster::GetInstance()->UnSubscribe(this, 0);
}

void FirstTimeFinishLevelState::InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor) 
{
	myStateStack = aStateStackProxy;
	myController = aController;
	myCursor = aCursor;
	myStateStatus = eStateStatus::eKeepState;
	myIsActiveState = true;
	myIsLetThrough = true;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_first_time_score_screen.xml", nullptr, -1);
	myCursor->SetShouldRender(true);
	InitControllerInMenu(myController, myGUIManager, myCursor);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	myController->SetIsInMenu(true);
	GC::FirstTimeScoreSubmit = true;
	std::ofstream file(CU::GetMyDocumentFolderPath() + "Data\\Setting\\SET_game_setting.bin", std::ios::binary | std::ios::out);
	if (file.is_open() == true)
	{
		file.write(reinterpret_cast<const char*>(&GC::FirstTimeScoreSubmit), sizeof(bool));
		file.write(reinterpret_cast<const char*>(&GC::OptionsEnableOffline), sizeof(bool));
		file.close();
	}
}

void FirstTimeFinishLevelState::EndState() 
{
	myIsActiveState = false;
	myCursor->SetShouldRender(false);
}

const eStateStatus FirstTimeFinishLevelState::Update(const float& aDeltaTime) 
{
	HandleControllerInMenu(myController, myGUIManager, myCursor);

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void FirstTimeFinishLevelState::Render() 
{
	if (myRenderFlag == true)
	{
		myGUIManager->Render();
	}
}

void FirstTimeFinishLevelState::ResumeState() 
{
	myIsActiveState = true;
	myCursor->SetShouldRender(true);
	InitControllerInMenu(myController, myGUIManager, myCursor);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	myController->SetIsInMenu(true);
	myRenderFlag = true;
}

void FirstTimeFinishLevelState::PauseState() 
{
	PostMaster::GetInstance()->UnSubscribe(this, eMessageType::ON_CLICK);
}

void FirstTimeFinishLevelState::OnResize(int aWidth, int aHeight) 
{
	myGUIManager->OnResize(aWidth, aHeight);
}

void FirstTimeFinishLevelState::ReceiveMessage(const OnClickMessage& aMessage)
{
	switch (aMessage.myEvent)
	{
	case eOnClickEvent::GAME_QUIT:
		myStateStatus = eStateStatus::ePopMainState;
		break;
	case eOnClickEvent::OPTIONS_TOGGLE_OFFLINE_MODE:
		SET_RUNTIME(false);
		myStateStack->PushSubGameState(new ScoreState(myScores, myScoreInfo, myLevelID));
		myIsActiveState = false;
		myRenderFlag = false;
		break;
	}
}