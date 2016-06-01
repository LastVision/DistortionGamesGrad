#pragma once

class Entity;

struct ReachedGoalNote
{
	ReachedGoalNote(Entity* aGoal)
		: myGoal(aGoal){}
	const Entity* myGoal;
};