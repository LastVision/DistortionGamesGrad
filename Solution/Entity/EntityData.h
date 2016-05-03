#pragma once

#include "AnimationComponentData.h"
#include "GraphicsComponentData.h"
#include "PhysicsComponentData.h"
#include "TriggerComponentData.h"
#include "SoundComponentData.h"
#include "InputComponentData.h"
#include "SawBladeComponentData.h"
#include "SpikeComponentData.h"

struct EntityData
{
	eEntityType myType;
	AnimationComponentData myAnimationData;
	GraphicsComponentData myGraphicsData;
	PhysicsComponentData myPhysicsData;
	TriggerComponentData myTriggerData;
	SoundComponentData mySoundData;
	InputComponentData myInputData;
	SawBladeComponentData mySawBladeData;
	SpikeComponentData mySpikeData;
	std::string mySubType;
};