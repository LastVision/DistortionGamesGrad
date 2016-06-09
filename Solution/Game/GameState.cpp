#include "stdafx.h"
#include <Cursor.h>
#include <ControllerInput.h>
#include "GameState.h"
#include <GUIManager.h>

void GameState::HandleControllerInMenu(CU::ControllerInput* aController, GUI::GUIManager* aManager, GUI::Cursor* aCursor)
{
	if (aController->IsConnected() && aManager->IsPaused() == false)
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

		if (controllerX >= 0.5f)
		{
			if (myControllerPressedRight == false)
			{
				myControllerPressedRight = true;
				aManager->SelectButtonRight();
			}
		}
		else
		{
			myControllerPressedRight = false;
		}

		if (controllerX <= -0.5f)
		{
			if (myControllerPressedLeft == false)
			{
				myControllerPressedLeft = true;
				aManager->SelectButtonLeft();
			}
		}
		else
		{
			myControllerPressedLeft = false;
		}

		if (myControllerPressedDown == true || myControllerPressedLeft == true
			|| myControllerPressedRight == true || myControllerPressedUp)
		{
			aCursor->SetIsUsingController(true);
			aManager->Unhover();
		}
	}
}

void GameState::InitControllerInMenu(CU::ControllerInput* aController, GUI::GUIManager* aManager, GUI::Cursor* aCursor)
{
	aController->Update(0.f);

	myControllerPressedLeft = false;
	myControllerPressedRight = false;
	myControllerPressedUp = false;
	myControllerPressedDown = false;

	if (aController->CheckConnection() == true)
	{
		aManager->HoverSelectedButtonWithoutSound();
		aCursor->SetIsUsingController(true);
	}
}