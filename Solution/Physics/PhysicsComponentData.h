#pragma once
#include <GameEnum.h>
#include <Vector.h>

struct PhysicsComponentData
{
	bool myExistsInEntity = false;
	bool myIsActiveFromStart = true;
	bool myIsSphere = false;
	ePhysics myPhysicsType = ePhysics::_NONE;
	CU::Vector3<float> myPhysicsMin;
	CU::Vector3<float> myPhysicsMax;
	std::string myObjPath = "";
};