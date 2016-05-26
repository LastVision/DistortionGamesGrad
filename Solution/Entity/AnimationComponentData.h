#pragma once
#include <string>

struct AnimationComponentData
{
	bool myExistsInEntity = false;
	std::string myModelPath = "";
	std::string mySecondAnimationPath = "";
	std::string myEffectPath = "";
};