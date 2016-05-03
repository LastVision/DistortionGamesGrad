#pragma once

#include "AnimationComponentData.h"
#include "GraphicsComponentData.h"
#include "InputComponentData.h"
#include "MovementComponentData.h"
#include "PhysicsComponentData.h"
#include "TriggerComponentData.h"
#include "SoundComponentData.h"
#include "SawBladeComponentData.h"
#include "SpikeComponentData.h"

struct EntityData
{
	eEntityType myType;
	AnimationComponentData myAnimationData;
	GraphicsComponentData myGraphicsData;
	InputComponentData myInputData;
	MovementComponentData myMovementData;
	PhysicsComponentData myPhysicsData;
	TriggerComponentData myTriggerData;
	SoundComponentData mySoundData;
	SawBladeComponentData mySawBladeData;
	SpikeComponentData mySpikeData;
	std::string mySubType;
};