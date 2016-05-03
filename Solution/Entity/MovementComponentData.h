#pragma once

struct MovementComponentData
{
	bool myExistsInEntity = false;
	float myGravity;
	float myImpulse;
	float myRotationSpeed;
	CU::Vector2<float> myDrag;
};