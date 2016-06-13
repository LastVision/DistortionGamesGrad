#include "stdafx.h"

#include "AreYouSureState.h"
#include <AudioInterface.h>
#include <ControllerInput.h>
#include <Cursor.h>
#include <FadeMessage.h>
#include <GUIManager.h>
#include <HatManager.h>
#include <InputWrapper.h>
#include <OnClickMessage.h>
#include <PostMaster.h>
#include <Text.h>

AreYouSureState::AreYouSureState()
{
}


AreYouSureState::~AreYouSureState()
{
	SAFE_DELETE(myGUIManager);
	myStateStack = nullptr;
	myCursor = nullptr;
	myController = nullptr;
	PostMaster::GetInstance()->UnSubscribe(this, 0);
}

void AreYouSureState::InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor)
{
	myStateStack = aStateStackProxy;
	myController = aController;
	myCursor = aCursor;
	myStateStatus = eStateStatus::eKeepState;
	myIsActiveState = true;
	myIsLetThrough = true;
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_options_reset_are_you_sure.xml", nullptr, -1);
	myCursor->SetShouldRender(true);
	InitControllerInMenu(myController, myGUIManager, myCursor);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	myController->SetIsInMenu(true);

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}
void AreYouSureState::EndState()
{
	myIsActiveState = false;
	myCursor->SetShouldRender(false);
}

const eStateStatus AreYouSureState::Update(const float& aDeltaTime)
{
	HandleControllerInMenu(myController, myGUIManager, myCursor);

	myGUIManager->Update(aDeltaTime);

	return myStateStatus;
}
void AreYouSureState::Render()
{
	myGUIManager->Render();
}
void AreYouSureState::ResumeState()
{
	myIsActiveState = true;
	myCursor->SetShouldRender(true);
	InitControllerInMenu(myController, myGUIManager, myCursor);
	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);
	myController->SetIsInMenu(true);

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}
void AreYouSureState::PauseState()
{
	PostMaster::GetInstance()->UnSubscribe(this, eMessageType::ON_CLICK);
}
void AreYouSureState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

void AreYouSureState::ReceiveMessage(const OnClickMessage& aMessage)
{
	switch (aMessage.myEvent)
	{
	case eOnClickEvent::OPTIONS_RESET_ARE_YOU_SURE_NO:
		myStateStatus = eStateStatus::ePopSubState;
		break;
	case eOnClickEvent::OPTIONS_RESET_ARE_YOU_SURE_YES:
		GC::Gold = 0;
		std::string file = CU::GetMyDocumentFolderPath() + "/Data/save.bin";
		if (CU::FileExists(file) == true)
		{
			DeleteFile(file.c_str());
		}
		file = CU::GetMyDocumentFolderPath() + "/Data/UnlockedHats.bin";
		if (CU::FileExists(file) == true)
		{
			//DeleteFile(file.c_str());
			HatManager::GetInstance()->Reset();
		}
		file = CU::GetMyDocumentFolderPath() + "/Data/UnlockedLevels.bin";
		if (CU::FileExists(file) == true)
		{
			DeleteFile(file.c_str());
		}
		file = CU::GetMyDocumentFolderPath() + "/Data/UnlockedLevels_Nightmare.bin";
		if (CU::FileExists(file) == true)
		{
			DeleteFile(file.c_str());
		}
		for (int i = 1; i <= 20; ++i)
		{
			file = CU::GetMyDocumentFolderPath() + "/Data/Score/Score" + std::to_string(i) + ".bin";
			if (CU::FileExists(file) == true)
			{
				DeleteFile(file.c_str());
			}
		}
		for (int i = 1; i <= 12; ++i)
		{
			file = CU::GetMyDocumentFolderPath() + "/Data/Score/Score_Nightmare" + std::to_string(i) + ".bin";
			if (CU::FileExists(file) == true)
			{
				DeleteFile(file.c_str());
			}
		}
		myStateStatus = eStateStatus::ePopSubState;
		break;
	}
}