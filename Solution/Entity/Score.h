#pragma once

struct Score
{
	Score()
		: myDeathCount(0)
		, myTime(0)
		, myActive(false)
	{}

	int myDeathCount;
	float myTime;
	bool myActive;
};