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
			if (myControllerYUpIsDown == false)
			{
				myControllerYUpIsDown = true;
				aManager->SelectPreviousButtonX();
			}
		}
		else
		{
			myControllerYUpIsDown = false;
		}

		if (controllerY <= -0.5f)
		{
			if (myControllerYDownIsDown == false)
			{
				myControllerYDownIsDown = true;
				aManager->SelectNextButtonX();
			}
		}
		else
		{
			myControllerYDownIsDown = false;
		}

		if (controllerX >= 0.5f)
		{
			if (myControllerXUpIsDown == false)
			{
				myControllerXUpIsDown = true;
				aManager->SelectNextButtonY();
			}
		}
		else
		{
			myControllerXUpIsDown = false;
		}

		if (controllerX <= -0.5f)
		{
			if (myControllerXDownIsDown == false)
			{
				myControllerXDownIsDown = true;
				aManager->SelectPreviousButtonY();
			}
		}
		else
		{
			myControllerXDownIsDown = false;
		}
	}
}

void GameState::InitControllerInMenu(CU::ControllerInput* aController, GUI::GUIManager* aManager)
{
	aController->Update(0.f);

	myControllerXUpIsDown = false;
	myControllerXDownIsDown = false;
	myControllerYUpIsDown = false;
	myControllerYDownIsDown = false;

	if (aController->CheckConnection())
	{
		aManager->HoverSelectedButton();
	}
}