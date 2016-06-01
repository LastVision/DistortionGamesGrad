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
	if (myScore.myReachedGoal == false)
	{
		myScore.myTime += aDeltaTime;
	}
}

void ScoreComponent::ReceiveNote(const DeathNote&)
{
	++myScore.myDeathCount;
}

void ScoreComponent::ReceiveNote(const SpawnNote&)
{
	myScore.myActive = true;
	myScore.myTime = 0;
}

void ScoreComponent::ReachedGoal()
{
	myScore.myReachedGoal = true;
}