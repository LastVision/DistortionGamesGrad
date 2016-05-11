#include "stdafx.h"
#include "Movement.h"
#include "PhysicsComponent.h"

Movement::Movement(const MovementComponentData& aData, CU::Matrix44f& anOrientation, MovementComponent& aMovementComponent)
	: myData(aData)
	, myOrientation(anOrientation)
	, myMovementComponent(aMovementComponent)
	, myIsActive(false)
	, myIsInSteam(false)
{
	myRaycastHandler = [=](PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
	{
		this->HandleRaycast(aComponent, aDirection, aHitPosition, aHitNormal);
	};
}


Movement::~Movement()
{
}
