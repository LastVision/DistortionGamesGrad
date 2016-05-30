#pragma once

#include <Vector.h>

namespace Prism
{
	class Texture;

	struct DecalTextures
	{
		Texture* myTexture;
		Texture* myMetalness;
		Texture* myRoughness;
		Texture* myNormalMap;
	};

	struct DecalInfo
	{
		DecalTextures* myTextures;
		CU::Vector3<float> myPosition;
		CU::Vector3<float> myDirection;

		float myTime;
		bool myIsFading;
	};
}