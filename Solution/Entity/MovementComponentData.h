#pragma once

struct MovementComponentData
{
	bool myExistsInEntity = false;
	float myGravity = 0.f;
	float myImpulse = 0.f;
	float myRotationSpeed = 0.f;
	float myDeadZone = 0.f;
	float myAngleEpsilon = 0.f;
	float myWalkSpeedWithLegs = 0.f;
	float myWalkSpeedWithoutLegs = 0.f;
	float myWalkDrag = 0.f;
	float myDashAimTime = 0.f;
	float myDashFlyTime = 0.f;
	float myDashDistance = 0.f;
	float myDashCooldown = 0.f;
	float myDashSpeed = 0.f;
	float myMaxAngleWhenLanding = 0.f;
	CU::Vector2<float> myDrag;
};