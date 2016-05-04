#pragma once

struct MovementComponentData
{
	bool myExistsInEntity = false;
	float myGravity;
	float myImpulse;
	float myRotationSpeed;
	float myDeadZone;
	float myAngleEpsilon;
	float myWalkSpeed;
	float myWalkDrag;
	CU::Vector2<float> myDrag;
};