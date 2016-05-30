#pragma once

#include <Vector.h>

namespace Prism
{
	class Texture;
	struct DecalInfo
	{
		Texture* myTexture;
		Texture* myMetalness;
		Texture* myRoughness;
		Texture* myNormalMap;
		CU::Vector3<float> myPosition;
		CU::Vector3<float> myDirection;

		float myTime;
		bool myIsFading;
	};
}