#include "stdafx.h"
#include "InputComponent.h"
#include <ControllerInput.h>


InputComponent::InputComponent(Entity& aEntity, const InputComponentData& aInputData, CU::Matrix44f& anOrientation)
	: Component(aEntity)
	, myComponentData(aInputData)
	, myOrientation(anOrientation)
{
	myController = new CU::ControllerInput(0);

}

InputComponent::~InputComponent()
{
	SAFE_DELETE(myController);
}

void InputComponent::Update(float aDeltaTime)
{

	if (myController->IsConnected() == true)
	{
		if (myController->ButtonOnDown(CU::eXboxButton::A))
		{
			myOrientation.SetPos(CU::Vector3f(0, 1, 0));
		}
		else if (myController->LeftThumbstickX() > 0.5f)
		{
			myOrientation.SetPos(CU::Vector3f(1, 0, 0));
		}
		else if (myController->LeftThumbstickX() < -0.5f)
		{
			myOrientation.SetPos(CU::Vector3f(-1, 0, 0));
		}
		else
		{
			myOrientation.SetPos(CU::Vector3f(0, 0, 0));
		}

	}





}
