#include "stdafx.h"
#include <ButtonWidget.h>
#include <ControllerInput.h>
#include <Cursor.h>
#include <FadeMessage.h>
#include <InputWrapper.h>
#include <GUIManager.h>
#include "LevelSelectState.h"
#include <OnClickMessage.h>
#include <PostMaster.h>
#include <WidgetContainer.h>

LevelSelectState::LevelSelectState(bool aIsNightmare)
	: myIsNightmare(aIsNightmare)
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

	if (myIsNightmare == false)
	{
		myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_level_select.xml", nullptr, -1);
	}
	else
	{
		myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_nightmare_level_select.xml", nullptr, -1);
		myGUIManager->CheckUnlockedNightmareLevels();
	}

	myCursor->SetShouldRender(true);
	InitControllerInMenu(myController, myGUIManager, myCursor);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);

	//RetrieveUnlockedLevelsFromFile();
	myController->SetIsInMenu(true);

#ifdef RELEASE_BUILD
	if (myIsNightmare == false && GC::HasWonGame == false)
	{
		int size = static_cast<GUI::WidgetContainer*>(myGUIManager->GetWidgetContainer()->At(0))->GetSize();
		static_cast<GUI::ButtonWidget*>(static_cast<GUI::WidgetContainer*>(myGUIManager->GetWidgetContainer()->At(0))->At(size - 2))->SetActive(false);
	}
#endif

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void LevelSelectState::EndState()
{
	myIsActiveState = false;
	myCursor->SetShouldRender(false);
}

const eStateStatus LevelSelectState::Update(const float& aDeltaTime)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true || myController->ButtonOnDown(eXboxButton::BACK)
		|| myController->ButtonOnDown(eXboxButton::B))
	{
		myIsActiveState = false;
		myCursor->SetShouldRender(false);
		return eStateStatus::ePopMainState;
	}

	HandleControllerInMenu(myController, myGUIManager, myCursor);

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}

void LevelSelectState::Render()
{
	myGUIManager->Render();
}

void LevelSelectState::ResumeState()
{
	bool runtime = GET_RUNTIME;
	SET_RUNTIME(false);
	SAFE_DELETE(myGUIManager);

	if (myIsNightmare == false)
	{
		myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_level_select.xml", nullptr, -1);
	}
	else
	{
		myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_nightmare_level_select.xml", nullptr, -1);
		myGUIManager->CheckUnlockedNightmareLevels();
	}
	SET_RUNTIME(runtime);

	myIsActiveState = true;
	myCursor->SetShouldRender(true);
	InitControllerInMenu(myController, myGUIManager, myCursor);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	myController->SetIsInMenu(true);
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
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

CU::GrowingArray<bool> LevelSelectState::RetrieveUnlockedLevelsFromFile()
{
	CU::GrowingArray<bool> toReturn(64);

	std::fstream file;
	file.open(CU::GetMyDocumentFolderPath() + "Data/UnlockedLevels.bin", std::ios::binary | std::ios::in);

	if (file.is_open() == true)
	{
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
	}
	return toReturn;
}

const int LevelSelectState::GetAmountOfStarsFromFile(const int aLevelID)
{
	int toReturn = 0;
	std::fstream file;
	file.open(CU::GetMyDocumentFolderPath() + "Data/Score/Score" + std::to_string(aLevelID) + ".bin", std::ios::binary | std::ios::in);

	if (file.is_open() == true)
	{
		int levelID = 0;
		float time = 0;
		int stars = 0;
		bool isEndOfFile = false;
		while (isEndOfFile == false)
		{
			if (file.eof())
			{
				isEndOfFile = true;
				break;
			}
			file >> levelID >> time >> stars;
			toReturn = stars;
		}

		file.close();
	}
	return toReturn;
}