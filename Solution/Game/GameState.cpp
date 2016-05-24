#include "stdafx.h"

#include "GameState.h"
#include <ControllerInput.h>
#include <GUIManager.h>

void GameState::HandleControllerInMenu(CU::ControllerInput* aController, GUI::GUIManager* aManager)
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
	}
}

void GameState::InitControllerInMenu(CU::ControllerInput* aController, GUI::GUIManager* aManager)
{
	aController->Update(0.f);

	myControllerPressedLeft = false;
	myControllerPressedRight = false;
	myControllerPressedUp = false;
	myControllerPressedDown = false;

	if (aController->CheckConnection())
	{
		aManager->HoverSelectedButton();
	}
}