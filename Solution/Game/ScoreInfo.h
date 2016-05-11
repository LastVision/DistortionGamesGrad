#pragma once

struct ScoreInfo
{
	ScoreInfo(float aShortTime = 5.f, float aMediumTime = 15.f, float aLongTime = 25.f)
		: myShortTime(aShortTime)
		, myMediumTime(aMediumTime)
		, myLongTime(aLongTime)
	{}

	float myShortTime;
	float myMediumTime;
	float myLongTime;
};