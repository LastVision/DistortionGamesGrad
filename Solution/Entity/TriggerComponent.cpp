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
	, myHasTriggered(false)
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

int TriggerComponent::GetValue() const
{
	return myData.myValue;
}

bool TriggerComponent::GetIsActiveFromStart() const
{
	return myData.myIsActiveFromStart;
}