#include "stdafx.h"
#include <ControllerInput.h>
#include <Cursor.h>
#include "HatUnlockState.h"
#include <HatManager.h>
#include <GUIManager.h>
#include <InputWrapper.h>
#include <ModelLoader.h>
#include <OnClickMessage.h>
#include <PostMaster.h>
#include <SpriteProxy.h>

HatUnlockState::HatUnlockState()
	: myGUIManager(nullptr)
	, myIsSpinning(false)
	, mySpinTimer(0.f)
	, myMaxSpinTime(10.f)
	, myCurrentHatToWin(0)
	, myHats(8)
	, myHatWon(nullptr)
	, myHasWonAllHats(false)
	, myMoveAmount(0.f)
{
}

HatUnlockState::~HatUnlockState()
{
	PostMaster::GetInstance()->UnSubscribe(this, 0);
	SAFE_DELETE(myGUIManager);
	myCursor = nullptr;

	for (int i = 0; i < myHats.Size(); ++i)
	{
		SAFE_DELETE(myHats[i].mySprite);
	}

	SAFE_DELETE(myHatWon);
	SAFE_DELETE(mySpinBox);
	SAFE_DELETE(myAllHatsWonText);
}

void HatUnlockState::InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor)
{
	myIsActiveState = true;
	myIsLetThrough = false;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myCursor = aCursor;
	myController = aController;
	myCursor->SetShouldRender(true);
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_hat_unlock.xml", nullptr, -1);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);

	InitControllerInMenu(myController, myGUIManager, myCursor);
	myController->SetIsInMenu(true);

	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);

	CU::Vector2<float> size(256.f, 256.f);
	std::string hatPath("Data/Resource/Texture/Hats/T_hat");
	for (int i = 0; i < HatManager::GetInstance()->GetAmountOfHats(); ++i)
	{
		if (HatManager::GetInstance()->IsHatUnlocked(i) == false)
		{
			myHats.Add(HatUnlock(Prism::ModelLoader::GetInstance()->LoadSprite(hatPath + std::to_string(i) + ".dds", size, size * 0.5f), i));
		}
	}
	if (myHats.Size() <= 0)
	{
		myHasWonAllHats = true;
	}

	mySpinBox = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/Hat/T_spin_box.dds", { 1024.f, 512.f }, { 512.f, 256.f });
	myAllHatsWonText = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/Hat/T_all_hats_won.dds"
		, { size.x * 2.f, size.y }, { size.x, size.y * 0.5f });

	if (myHats.Size() == 1)
	{
		myLeftIndex = 0;
		myMiddleIndex = 0;
	}
	else
	{
		myLeftIndex = 0;
		myMiddleIndex = 1;
	}
}

void HatUnlockState::EndState()
{
}

void HatUnlockState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

const eStateStatus HatUnlockState::Update(const float& aDeltaTime)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true || myController->ButtonOnDown(eXboxButton::BACK)
		|| myController->ButtonOnDown(eXboxButton::B))
	{
		myIsActiveState = false;
		myCursor->SetShouldRender(false);
		return eStateStatus::ePopSubState;
	}

	HandleControllerInMenu(myController, myGUIManager);
	myGUIManager->Update(aDeltaTime);

	if (myIsSpinning == true)
	{

		mySpinTimer -= aDeltaTime;

		myMoveAmount = myTweener.DoTween(mySpinTimer, 12288.f, -12288.f, myMaxSpinTime, eTweenType::EXPONENTIAL);


		myLeftIndex = int(int(myMoveAmount) / 256) % myHats.Size();
		myMiddleIndex = myLeftIndex - 1;
		if (myMiddleIndex < 0)
		{
			myMiddleIndex = myHats.Size() - 1;
		}
		myCurrentHatToWin = myLeftIndex;

		if (mySpinTimer < -5.f)
		{
			WinHat(myHats[myCurrentHatToWin].myID);
		}
	}




	return myStateStatus;
}

void HatUnlockState::Render()
{
	myGUIManager->Render();
	CU::Vector2<float> windowSize = Prism::Engine::GetInstance()->GetWindowSize() * 0.5f;

	myRenderPosition = windowSize;
	myRenderPosition.x -= 256.f;
	myRenderPosition.x += fmod(myMoveAmount, 256.f);
	if (myIsSpinning == true)
	{
		myHats[myLeftIndex].mySprite->Render(myRenderPosition);
		myRenderPosition.x += 256.f;
		myHats[myMiddleIndex].mySprite->Render(myRenderPosition);

	}
	else if (myHatWon != nullptr)
	{
		myHatWon->Render(windowSize);
	}

	if (myHasWonAllHats == true)
	{
		myAllHatsWonText->Render(windowSize);
	}
	else
	{
		mySpinBox->Render(windowSize);
	}
}

void HatUnlockState::ResumeState()
{
	InitControllerInMenu(myController, myGUIManager, myCursor);
	myController->SetIsInMenu(true);
}

void HatUnlockState::PauseState()
{

}

void HatUnlockState::ReceiveMessage(const OnClickMessage& aMessage)
{
	switch (aMessage.myEvent)
	{
	case eOnClickEvent::SPIN:
		if (myIsSpinning == false)
		{
			if (myHats.Size() > 0)
			{
				Spin();
			}
			else
			{
				myHasWonAllHats = true;
			}
		}
		break;
	case eOnClickEvent::GAME_QUIT:
		myStateStatus = eStateStatus::ePopSubState;
		break;
	}
}

void HatUnlockState::Spin()
{
	myIsSpinning = true;
	mySpinTimer = myMaxSpinTime;
}

void HatUnlockState::WinHat(int aHatID)
{
	myIsSpinning = false;
	HatManager::GetInstance()->UnlockHat(aHatID);


	for (int i = myHats.Size() - 1; i >= 0; --i)
	{
		if (aHatID == myHats[i].myID)
		{
			SAFE_DELETE(myHatWon);
			myHatWon = myHats[i].mySprite;
			myHats.RemoveCyclicAtIndex(i);
			break;
		}
	}
	//ta bort r�tt hatt ur my hats!!!
}

void HatUnlockState::WrapIndex()
{
	--myMiddleIndex;

	if (myLeftIndex < 0)
	{
		myLeftIndex = myHats.Size() - 1;
	}
	if (myMiddleIndex < 0)
	{
		myMiddleIndex = myHats.Size() - 1;
	}
}