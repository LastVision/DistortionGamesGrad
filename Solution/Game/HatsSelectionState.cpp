#include "stdafx.h"
#include <ControllerInput.h>
#include <Cursor.h>
#include <FadeMessage.h>
#include "HatsSelectionState.h"
#include <HatManager.h>
#include <GUIManager.h>
#include <InputWrapper.h>
#include <ModelLoader.h>
#include <OnClickMessage.h>
#include <PostMaster.h>
#include <SpriteProxy.h>
#include <SQLWrapper.h>

HatsSelectionState::HatsSelectionState()
	: myGUIManager(nullptr)
		, myHats(8)
		, mySecondControllerPressedLeft(false)
		, mySecondControllerPressedRight(false)
		, myLeftArrow(nullptr)
		, myRightArrow(nullptr)
		, myArrowBox(nullptr)
		, myUVScrollingTime(0.f)
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
		SAFE_DELETE(myHats[i].mySprite);
	}
	SAFE_DELETE(mySecondController);
	SAFE_DELETE(myLockSprite);
	SAFE_DELETE(myLeftArrow);
	SAFE_DELETE(myRightArrow);
	SAFE_DELETE(myArrowBox);
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
	for (int i = 0; i < HatManager::GetInstance()->GetAmountOfHats(); ++i)
	{
		myHats.Add(HatStruct(Prism::ModelLoader::GetInstance()->LoadSprite(hatPath + std::to_string(i) + ".dds", size, size * 0.5f), i));
	}

	myLockSprite = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Hats/T_lock.dds", size, size * 0.5f);

	myLeftArrow = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/Hat/T_left_arrow.dds", size * 0.5f, size * 0.25f);
	myRightArrow = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/Hat/T_right_arrow.dds", size * 0.5f, size * 0.25f);
	myArrowBox = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/Menu/Hat/T_arrow_box.dds", size * 0.5f, size * 0.25f);

	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
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
		CU::SQLWrapper sql;
		sql.Connect("server.danielcarlsson.net", "Test@d148087", "DGames2016", "danielcarlsson_net_db_1", CLIENT_COMPRESS | CLIENT_FOUND_ROWS | CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS);
		sql.WriteHatStat(HatManager::GetInstance()->GetHatIDOnPlayer(1));
		sql.WriteHatStat(HatManager::GetInstance()->GetHatIDOnPlayer(2));
		return eStateStatus::ePopSubState;
	}
	myUVScrollingTime += aDeltaTime;
	HandleControllerInSelection(myController, myGUIManager, myCursor);
	myController->Update(aDeltaTime);
	mySecondController->Update(aDeltaTime);
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
			do
			{
				++aCurrentPlayerHat;
				if (aCurrentPlayerHat >= HatManager::GetInstance()->GetAmountOfHats())
				{
					aCurrentPlayerHat = -1;
					break;
				}
			} while (HatManager::GetInstance()->IsHatUnlocked(myHats[aCurrentPlayerHat].myHatID) == false);
			
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
			do
			{
				--aCurrentPlayerHat;
				if (aCurrentPlayerHat == -1)
				{
					break;
				}
				if (aCurrentPlayerHat < -1)
				{
					aCurrentPlayerHat = HatManager::GetInstance()->GetAmountOfHats() - 1;
				}
			} while (HatManager::GetInstance()->IsHatUnlocked(myHats[aCurrentPlayerHat].myHatID) == false);

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
	CU::Vector2<float> uvOffset(myUVScrollingTime, 0.f);
	myLeftArrow->SetUVOutsideZeroToOne({ uvOffset.x, 0.f }, { uvOffset.x + 1.f, 1.f });
	myRightArrow->SetUVOutsideZeroToOne({ -uvOffset.x, 0.f }, { -uvOffset.x + 1.f, 1.f });
	myGUIManager->Render();
	CU::Vector2<float> leftOffset(-128.f, -32.f);
	CU::Vector2<float> rightOffset(128.f, -32.f);
	CU::Vector2<float> windowSize = Prism::Engine::GetInstance()->GetWindowSize() * 0.5f;
	
	CU::Vector2<float> playerOneRenderPos(windowSize.x - 256.f, windowSize.y);
	myPlayerOnePortrait->Render(playerOneRenderPos);
	myLeftArrow->Render(playerOneRenderPos + leftOffset);
	myRightArrow->Render(playerOneRenderPos + rightOffset);
	myArrowBox->Render(playerOneRenderPos + leftOffset);
	myArrowBox->Render(playerOneRenderPos + rightOffset);

	if (myPlayerOneCurrentHat != -1)
	{
		myHats[myPlayerOneCurrentHat].mySprite->Render(playerOneRenderPos);
		if (myPlayerOneCurrentHat != -1 && HatManager::GetInstance()->IsHatUnlocked(myPlayerOneCurrentHat) == false)
		{
			myLockSprite->Render(playerOneRenderPos);
		}
	}
	CU::Vector2<float> playerTwoRenderPos(windowSize.x + 256.f, windowSize.y);
	myPlayerTwoPortrait->Render(playerTwoRenderPos);
	myLeftArrow->Render(playerTwoRenderPos + leftOffset);
	myRightArrow->Render(playerTwoRenderPos + rightOffset);
	myArrowBox->Render(playerTwoRenderPos + leftOffset);
	myArrowBox->Render(playerTwoRenderPos + rightOffset);
	if (myPlayerTwoCurrentHat != -1)
	{
		myHats[myPlayerTwoCurrentHat].mySprite->Render(playerTwoRenderPos);
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
	PostMaster::GetInstance()->SendMessage(FadeMessage(1.f / 3.f));
}

void HatsSelectionState::PauseState()
{

}

void HatsSelectionState::ReceiveMessage(const OnClickMessage& aMessage)
{
	switch (aMessage.myEvent)
	{
	case eOnClickEvent::HAT_QUIT:
		CU::SQLWrapper sql;
		sql.Connect("server.danielcarlsson.net", "Test@d148087", "DGames2016", "danielcarlsson_net_db_1", CLIENT_COMPRESS | CLIENT_FOUND_ROWS | CLIENT_MULTI_STATEMENTS | CLIENT_MULTI_RESULTS);
		sql.WriteHatStat(HatManager::GetInstance()->GetHatIDOnPlayer(1));
		sql.WriteHatStat(HatManager::GetInstance()->GetHatIDOnPlayer(2));
		myStateStatus = eStateStatus::ePopSubState;
		break;
	}
}

void HatsSelectionState::HandleControllerInSelection(CU::ControllerInput* aController, GUI::GUIManager* aManager, GUI::Cursor* aCursor)
{
	if (aController->IsConnected())
	{
		if (aController->ButtonOnDown(eXboxButton::A))
		{
			aManager->PressSelectedButton();
		}

		float controllerX = aController->LeftThumbstickX();
		float controllerY = aController->LeftThumbstickY();

		if (controllerY >= 0.5f)
		{
			if (myControllerPressedUp == false)
			{
				myControllerPressedUp = true;
				aManager->SelectButtonUp();
			}
		}
		else
		{
			myControllerPressedUp = false;
		}

		if (controllerY <= -0.5f)
		{
			if (myControllerPressedDown == false)
			{
				myControllerPressedDown = true;
				aManager->SelectButtonDown();
			}
		}
		else
		{
			myControllerPressedDown = false;
		}

		

		if (myControllerPressedDown == true || myControllerPressedUp == true)
		{
			aCursor->SetIsUsingController(true);
			aManager->Unhover();
		}
	}
}