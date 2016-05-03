#include "stdafx.h"
#include <ControllerInput.h>
#include "InputComponent.h"
#include "MovementComponent.h"

InputComponent::InputComponent(Entity& aEntity, const InputComponentData& aInputData)
	: Component(aEntity)
	, myComponentData(aInputData)
	, myMovement(nullptr)
	, myController(new CU::ControllerInput(0))
{
}

InputComponent::~InputComponent()
{
	SAFE_DELETE(myController);
}

void InputComponent::Init()
{
	myMovement = myEntity.GetComponent<MovementComponent>();
	DL_ASSERT_EXP(myMovement != nullptr, "Input component needs movement component to work correctly.");
}

void InputComponent::Update(float aDeltaTime)
{

	if (myController->IsConnected() == true)
	{
		if (myController->ButtonWhileDown(CU::eXboxButton::A))
		{
			myMovement->Impulse();
		}
		else
		{
		}
	}
}
