#include "stdafx.h"
#include "Movement.h"


Movement::Movement(const MovementComponentData& aData, CU::Matrix44f& anOrientation, MovementComponent& aMovementComponent)
	: myData(aData)
	, myOrientation(anOrientation)
	, myMovementComponent(aMovementComponent)
{
}


Movement::~Movement()
{
}
