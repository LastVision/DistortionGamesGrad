#pragma once
#include <GameEnum.h>
#include <Vector.h>

struct PhysicsComponentData
{
	bool myExistsInEntity = false;
	bool myIsActiveFromStart = true;
	bool myIsSphere = true;
	ePhysics myPhysicsType;
	CU::Vector3<float> myPhysicsMin;
	CU::Vector3<float> myPhysicsMax;
};