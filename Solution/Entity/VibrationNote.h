#pragma once

struct VibrationNote
{
	VibrationNote(const unsigned short aRightMotorValue, const unsigned short aLeftMotorValue, const float aTime)
		: myRightMotorValue(aRightMotorValue)
		, myLeftMotorValue(aLeftMotorValue)
		, myTime(aTime)
		{}

	const unsigned short myRightMotorValue;
	const unsigned short myLeftMotorValue;
	const float myTime;
};