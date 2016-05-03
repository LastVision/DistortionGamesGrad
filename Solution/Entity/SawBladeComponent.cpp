#include "stdafx.h"
#include "SawBladeComponent.h"

SawBladeComponent::SawBladeComponent(Entity& anEntity)
	: Component(anEntity)
	, myIsPatrolling(false)
	, myPositions(8)
{
}

SawBladeComponent::~SawBladeComponent()
{
}