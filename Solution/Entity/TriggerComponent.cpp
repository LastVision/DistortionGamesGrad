#include "stdafx.h"
#include "CollisionNote.h"
#include <GameStateMessage.h>
#include "PhysicsComponent.h"
#include <PostMaster.h>
#include "TriggerComponent.h"
#include "TriggerComponentData.h"

TriggerComponent::TriggerComponent(Entity& anEntity, const TriggerComponentData& someData)
	: Component(anEntity)
	, myData(someData)
	, myTriggerType(eTriggerType(someData.myTriggerType))
{
}

TriggerComponent::~TriggerComponent()
{
}

void TriggerComponent::Update(float)
{
	
}

void TriggerComponent::ReceiveNote(const CollisionNote&)
{
	
}

void TriggerComponent::Activate()
{
}

int TriggerComponent::GetLevelID()
{
	return myLevelToChangeToID;
}

void TriggerComponent::SetLevelChangeID(int anID)
{
	myLevelToChangeToID = anID;
}
