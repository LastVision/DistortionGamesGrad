#pragma once
#include <string>

struct PlayerGraphicsComponentData
{
	bool myExistsInEntity = false;

	std::string myShader = "";
	std::string myAnimationShader = "";

	std::string myBody = "";
	std::string myHead = "";
	std::string myLeftLeg = "";
	std::string myRightLeg = "";

	std::string myIdleAnimation = "";
	std::string myWalkAnimation = "";
	std::string myFlyAnimation = "";
	std::string myDashAimAnimation = "";
	std::string myDashFlyAnimation = "";

	std::string myPlayerOneArrow = "";
	std::string myPlayerTwoArrow = "";
};