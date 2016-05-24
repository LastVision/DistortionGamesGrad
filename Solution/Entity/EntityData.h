#pragma once

#include "AnimationComponentData.h"
#include "BounceComponentData.h"
#include "GraphicsComponentData.h"
#include "InputComponentData.h"
#include "MovementComponentData.h"
#include "PhysicsComponentData.h"
#include "TriggerComponentData.h"
#include "SoundComponentData.h"
#include "SawBladeComponentData.h"
#include "ScoreComponentData.h"
#include "SteamComponentData.h"
#include "PlayerGraphicsComponentData.h"
#include "PlayerComponentData.h"

struct EntityData
{
	EntityData()
		: myShowDecal(true)
	{}

	eEntityType myType;
	AnimationComponentData myAnimationData;
	BounceComponentData myBounceData;
	GraphicsComponentData myGraphicsData;
	InputComponentData myInputData;
	MovementComponentData myMovementData;
	PhysicsComponentData myPhysicsData;
	TriggerComponentData myTriggerData;
	SoundComponentData mySoundData;
	SawBladeComponentData mySawBladeData;
	ScoreComponentData myScoreData;
	PlayerGraphicsComponentData myPlayerGraphicsData;
	SteamComponentData mySteamData;
	PlayerComponentData myPlayerData;
	std::string mySubType;
	bool myShowDecal;
};