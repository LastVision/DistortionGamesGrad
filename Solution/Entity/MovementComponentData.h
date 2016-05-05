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
	float myDashAimTime;
	float myDashFlyTime;
	float myDashSpeed;
	float myDashCooldown;
	CU::Vector2<float> myDrag;
};