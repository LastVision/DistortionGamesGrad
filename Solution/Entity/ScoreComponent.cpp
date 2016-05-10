#include "stdafx.h"
#include "ScoreComponent.h"
#include "PhysicsComponent.h"

ScoreComponent::ScoreComponent(Entity& anEntity)
	: Component(anEntity)
{
}

ScoreComponent::~ScoreComponent()
{
}

void ScoreComponent::Update(float aDeltaTime)
{
	myScore.myTime += aDeltaTime;
}