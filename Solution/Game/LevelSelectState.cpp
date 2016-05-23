#include "stdafx.h"
#include <ControllerInput.h>
#include <Cursor.h>
#include <InputWrapper.h>
#include "LevelSelectState.h"
#include <GUIManager.h>
#include <OnClickMessage.h>
#include <PostMaster.h>

LevelSelectState::LevelSelectState()
{
}

LevelSelectState::~LevelSelectState()
{
	SAFE_DELETE(myGUIManager);
	myStateStack = nullptr;
	myCursor = nullptr;
	myController = nullptr;
	PostMaster::GetInstance()->UnSubscribe(this, 0);
}

void LevelSelectState::InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor)
{
	myStateStack = aStateStackProxy;
	myController = aController;
	myCursor = aCursor;
	myStateStatus = eStateStatus::eKeepState;
	myIsActiveState = true;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_level_select.xml", nullptr, -1);
	myCursor->SetShouldRender(true);
	InitControllerInMenu(myController, myGUIManager);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);

	RetriveUnlockedLevelsFromFile();
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

	HandleControllerInMenu(myController, myGUIManager);

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void LevelSelectState::Render()
{
	myGUIManager->Render();
}

void LevelSelectState::ResumeState()
{
	myIsActiveState = true;
	myCursor->SetShouldRender(true);
	InitControllerInMenu(myController, myGUIManager);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
}

void LevelSelectState::PauseState()
{
	PostMaster::GetInstance()->UnSubscribe(this, eMessageType::ON_CLICK);
}

void LevelSelectState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

void LevelSelectState::ReceiveMessage(const OnClickMessage& aMessage)
{
	switch (aMessage.myEvent)
	{
	case eOnClickEvent::GAME_QUIT:
		myStateStatus = eStateStatus::ePopMainState;
		break;
	}
}

CU::GrowingArray<bool> LevelSelectState::RetriveUnlockedLevelsFromFile()
{
	CU::GrowingArray<bool> toReturn(64);

	std::fstream file;
	file.open(CU::GetMyDocumentFolderPath() + "Data/UnlockedLevels.bin", std::ios::binary | std::ios::in);

	int levelID;
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
		toReturn.Add(isUnlocked);
	}

	file.close();
	return toReturn;
}