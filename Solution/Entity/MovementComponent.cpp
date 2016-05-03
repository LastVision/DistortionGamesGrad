#include "stdafx.h"
#include "MovementComponent.h"
#include <ControllerInput.h>


MovementComponent::MovementComponent(Entity& aEntity, const MovementComponentData& aInputData, CU::Matrix44f& anOrientation)
	: Component(aEntity)
	, myComponentData(aInputData)
	, myOrientation(anOrientation)
{
	myController = new CU::ControllerInput(0);

}

MovementComponent::~MovementComponent()
{
	SAFE_DELETE(myController);
}

void MovementComponent::Update(float aDeltaTime)
{

	if (myController->IsConnected() == true)
	{
		if (myController->ButtonWhileDown(CU::eXboxButton::A))
		{
			//myOrientation.SetPos(CU::Vector3f(0, 1, 0));
		}
		else
		{
			//myOrientation.SetPos(CU::Vector3f(0, 0, 0));
		}
	}
}

void MovementComponent::Impulse()
{
	myOrientation.SetPos(myOrientation.GetPos() + CU::Vector3f(0, 0.001f, 0));
}