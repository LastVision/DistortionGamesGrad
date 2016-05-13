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

		float controllerY = aController->LeftThumbstickY();

		if (controllerY >= 0.5f)
		{
			if (myControllerUpIsDown == false)
			{
				myControllerUpIsDown = true;
				aManager->SelectPreviousButton();
			}
		}
		else
		{
			myControllerUpIsDown = false;
		}

		if (controllerY <= -0.5f)
		{
			if (myControllerDownIsDown == false)
			{
				myControllerDownIsDown = true;
				aManager->SelectNextButton();
			}
		}
		else
		{
			myControllerDownIsDown = false;
		}
	}
}