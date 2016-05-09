#include "stdafx.h"
#include <ControllerInput.h>
#include "InputComponent.h"
#include "MovementComponent.h"
#include <PostMaster.h>
#include <PlayerActiveMessage.h>
#include <OnDeathMessage.h>
InputComponent::InputComponent(Entity& aEntity, const InputComponentData& aInputData)
	: Component(aEntity)
	, myComponentData(aInputData)
	, myMovement(nullptr)
{
	PostMaster::GetInstance()->Subscribe(eMessageType::ON_DEATH, this);

}

InputComponent::~InputComponent()
{
	PostMaster::GetInstance()->UnSubscribe(eMessageType::ON_DEATH, this);
	SAFE_DELETE(myController);
}

void InputComponent::Init()
{
	myMovement = myEntity.GetComponent<MovementComponent>();
	DL_ASSERT_EXP(myMovement != nullptr, "Input component needs movement component to work correctly.");
}

void InputComponent::AddController(int anID)
{
	myController = new CU::ControllerInput(anID);
}

void InputComponent::Update(float aDeltaTime)
{
	myController->Update(aDeltaTime);

	if (myController->IsConnected() == true)
	{
		if (myIsActive == true)
		{
			if (myController->ButtonOnDown(eXboxButton::A))
			{
				myMovement->Impulse();
			}
			else
			{
			}

			if (myController->ButtonOnDown(eXboxButton::RTRIGGER))
			{
				myMovement->RightTriggerDown();
			}
			else if (myController->ButtonOnUp(eXboxButton::RTRIGGER))
			{
				myMovement->RightTriggerUp();
			}

			myMovement->SetDirectionTarget(CU::Vector2<float>(myController->LeftThumbstickX(), myController->LeftThumbstickY()));
		}
		else if (myIsActive == false)
		{
			if (myController->ButtonOnDown(eXboxButton::A))
			{
				PostMaster::GetInstance()->SendMessage(PlayerActiveMessage(true, myPlayerID));
				myIsActive = true;
				myMovement->Impulse();
			}
		}
	}
}

void InputComponent::SetPlayerID(int aPlayerID)
{
	myPlayerID = aPlayerID;
}

int InputComponent::GetPlayerID()
{
	return myPlayerID;
}

void InputComponent::ReceiveMessage(const OnDeathMessage& aMessage)
{
	if (myPlayerID == aMessage.myPlayerID)
	{
		myIsActive = false;
	}
}
