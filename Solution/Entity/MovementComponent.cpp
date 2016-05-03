#include "stdafx.h"
#include "MovementComponent.h"

MovementComponent::MovementComponent(Entity& aEntity, const MovementComponentData& aInputData, CU::Matrix44f& anOrientation)
	: Component(aEntity)
	, myData(aInputData)
	, myOrientation(anOrientation)
{
}

MovementComponent::~MovementComponent()
{
}

void MovementComponent::Update(float aDeltaTime)
{
	myVelocity.y += myData.myGravity * aDeltaTime;

	Drag(aDeltaTime);

	myOrientation.SetPos(myOrientation.GetPos() + CU::Vector3<float>(myVelocity, 0));

	myOrientation.SetPos(CU::Vector3<float>(myOrientation.GetPos().x, fmaxf(myOrientation.GetPos().y, 0), myOrientation.GetPos().z));

	if (myOrientation.GetPos().y == 0)
	{
		myVelocity.y = 0;
	}

	DEBUG_PRINT(myOrientation.GetPos());
}

void MovementComponent::Impulse()
{
	myVelocity += CU::Vector3<float>(CU::Vector3<float>(0, myData.myImpulse, 0) * myOrientation).GetVector2();
}

void MovementComponent::Rotate(float aValue)
{
	myOrientation = CU::Matrix44<float>::CreateRotateAroundZ(-aValue * myData.myRotationSpeed) * myOrientation;
}

void MovementComponent::Drag(float aDeltaTime)
{
	if (myVelocity.x > 0)
	{
		myVelocity.x = fmaxf(myVelocity.x - myData.myDrag.x * aDeltaTime, 0);
	}
	else if (myVelocity.x < 0)
	{
		myVelocity.x = fminf(myVelocity.x + myData.myDrag.x * aDeltaTime, 0);
	}
}