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

	CU::Vector3<float> position();

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
	myVelocity.y += myData.myImpulse;
	//myOrientation.SetPos(myOrientation.GetPos() + CU::Vector3f(0, 0.001f, 0));
}