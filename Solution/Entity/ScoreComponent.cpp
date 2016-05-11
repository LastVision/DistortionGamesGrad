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

void ScoreComponent::ReceiveNote(const DeathNote&)
{
	++myScore.myDeathCount;
}

void ScoreComponent::ReceiveNote(const SpawnNote&)
{
	myScore.myTime = 0;
}