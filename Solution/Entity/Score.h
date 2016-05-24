#pragma once

struct Score
{
	Score()
		: myDeathCount(0)
		, myTime(0)
		, myActive(false)
		, myReachedGoal(false)
	{}

	int myDeathCount;
	float myTime;
	bool myActive;
	bool myReachedGoal;
};