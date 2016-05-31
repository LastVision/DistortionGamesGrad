#include "stdafx.h"
#include <ControllerInput.h>
#include <Cursor.h>
#include "HatsSelectionState.h"
#include <HatManager.h>
#include <GUIManager.h>
#include <InputWrapper.h>
#include <ModelLoader.h>
#include <OnClickMessage.h>
#include <PostMaster.h>
#include <SpriteProxy.h>

HatsSelectionState::HatsSelectionState()
	: myGUIManager(nullptr)
		, myHats(8)
		, mySecondControllerPressedLeft(false)
		, mySecondControllerPressedRight(false)
{
}

HatsSelectionState::~HatsSelectionState()
{
	PostMaster::GetInstance()->UnSubscribe(this, 0);
	SAFE_DELETE(myGUIManager);
	myCursor = nullptr;

	SAFE_DELETE(myPlayerOnePortrait);
	SAFE_DELETE(myPlayerTwoPortrait);

	for (int i = 0; i < myHats.Size(); ++i)
	{
		SAFE_DELETE(myHats[i]);
	}
	SAFE_DELETE(mySecondController);
	SAFE_DELETE(myLockSprite);
}

void HatsSelectionState::InitState(StateStackProxy* aStateStackProxy, CU::ControllerInput* aController, GUI::Cursor* aCursor)
{
	myIsActiveState = true;
	myIsLetThrough = false;
	myStateStatus = eStateStatus::eKeepState;
	myStateStack = aStateStackProxy;
	myCursor = aCursor;
	myController = aController;
	mySecondController = new CU::ControllerInput(eControllerID::Controller2);
	myCursor->SetShouldRender(true);
	myGUIManager = new GUI::GUIManager(myCursor, "Data/Resource/GUI/GUI_hat_selection.xml", nullptr, -1);

	CU::Vector2<int> windowSize = Prism::Engine::GetInstance()->GetWindowSizeInt();
	OnResize(windowSize.x, windowSize.y);

	InitControllerInMenu(myController, myGUIManager, myCursor);
	myController->SetIsInMenu(true);

	PostMaster::GetInstance()->Subscribe(this, eMessageType::ON_CLICK);

	myPlayerOneCurrentHat = HatManager::GetInstance()->GetHatIDOnPlayer(1);
	myPlayerTwoCurrentHat = HatManager::GetInstance()->GetHatIDOnPlayer(2);

	CU::Vector2<float> size(128.f, 128.f);
	myPlayerOnePortrait = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Hats/T_portrait_player1.dds", size, size * 0.5f);
	myPlayerTwoPortrait = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Hats/T_portrait_player2.dds", size, size * 0.5f);

	std::string hatPath("Data/Resource/Texture/Hats/T_hat");
	for (int i = 0; i <= HatManager::GetInstance()->GetAmountOfHats(); ++i)
	{
		myHats.Add(Prism::ModelLoader::GetInstance()->LoadSprite(hatPath + std::to_string(i) + ".dds", size, size * 0.5f));
	}

	myLockSprite = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Hats/T_lock.dds", size, size * 0.5f);

}

void HatsSelectionState::EndState()
{
}

void HatsSelectionState::OnResize(int aWidth, int aHeight)
{
	myGUIManager->OnResize(aWidth, aHeight);
}

const eStateStatus HatsSelectionState::Update(const float& aDeltaTime)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_ESCAPE) == true || myController->ButtonOnDown(eXboxButton::BACK)
		|| myController->ButtonOnDown(eXboxButton::B))
	{
		myIsActiveState = false;
		myCursor->SetShouldRender(false);
		return eStateStatus::ePopSubState;
	}

	//HandleControllerInMenu(myController, myGUIManager);
	myController->Update(aDeltaTime);
	myGUIManager->Update(aDeltaTime);

	HandleHatSelection(myController, myPlayerOneCurrentHat, 1, myControllerPressedLeft, myControllerPressedRight);
	HandleHatSelection(mySecondController, myPlayerTwoCurrentHat, 2, mySecondControllerPressedLeft, mySecondControllerPressedRight);


	return myStateStatus;
}

void HatsSelectionState::HandleHatSelection(CU::ControllerInput* aController, int& aCurrentPlayerHat, int aPlayerID
	, bool& aControllerPressedLeft, bool& aControllerPressedRight)
{
	float controllerX = aController->LeftThumbstickX();

	if (controllerX >= 0.5f)
	{
		if (aControllerPressedRight == false)
		{
			aControllerPressedRight = true;
			++aCurrentPlayerHat;
			if (aCurrentPlayerHat >= HatManager::GetInstance()->GetAmountOfHats())
			{
				aCurrentPlayerHat = -1;
			}
			if (HatManager::GetInstance()->IsHatUnlocked(aCurrentPlayerHat) == true)
			{
				HatManager::GetInstance()->SetHatOnPlayer(aPlayerID, aCurrentPlayerHat);
			}
		}
	}
	else
	{
		aControllerPressedRight = false;
	}

	if (controllerX <= -0.5f)
	{
		if (aControllerPressedLeft == false)
		{
			aControllerPressedLeft = true;
			--aCurrentPlayerHat;
			if (aCurrentPlayerHat < -1)
			{
				aCurrentPlayerHat = HatManager::GetInstance()->GetAmountOfHats() - 1;
			}
			if (HatManager::GetInstance()->IsHatUnlocked(aCurrentPlayerHat) == true)
			{
				HatManager::GetInstance()->SetHatOnPlayer(aPlayerID, aCurrentPlayerHat);
			}
		}
	}
	else
	{
		aControllerPressedLeft = false;
	}
}

void HatsSelectionState::Render()
{
	myGUIManager->Render();

	CU::Vector2<float> windowSize = Prism::Engine::GetInstance()->GetWindowSize() * 0.5f;
	CU::Vector2<float> playerOneRenderPos(windowSize.x - 256.f, windowSize.y);
	myPlayerOnePortrait->Render(playerOneRenderPos);

	if (myPlayerOneCurrentHat != -1)
	{
		myHats[myPlayerOneCurrentHat]->Render(playerOneRenderPos);
		if (myPlayerOneCurrentHat != -1 && HatManager::GetInstance()->IsHatUnlocked(myPlayerOneCurrentHat) == false)
		{
			myLockSprite->Render(playerOneRenderPos);
		}
	}
	CU::Vector2<float> playerTwoRenderPos(windowSize.x + 256.f, windowSize.y);
	myPlayerTwoPortrait->Render(playerTwoRenderPos);
	if (myPlayerTwoCurrentHat != -1)
	{
		myHats[myPlayerTwoCurrentHat]->Render(playerTwoRenderPos);
		if (myPlayerTwoCurrentHat != -1 && HatManager::GetInstance()->IsHatUnlocked(myPlayerTwoCurrentHat) == false)
		{
			myLockSprite->Render(playerTwoRenderPos);
		}
	}

}

void HatsSelectionState::ResumeState()
{
	InitControllerInMenu(myController, myGUIManager, myCursor);
	myController->SetIsInMenu(true);
}

void HatsSelectionState::PauseState()
{

}

void HatsSelectionState::ReceiveMessage(const OnClickMessage& aMessage)
{
	switch (aMessage.myEvent)
	{
	case eOnClickEvent::GAME_QUIT:
		myStateStatus = eStateStatus::ePopSubState;
		break;
	}
}