#pragma once

#include <Vector.h>

namespace Prism
{
	class Texture;

	struct DecalTextures
	{
		Texture* myTexture;
		Texture* myEmissive;
		Texture* myMetalness;
		Texture* myRoughness;
		Texture* myNormalMap;
	};

	struct DecalInfo
	{
		DecalTextures* myTextures;
		CU::Vector3<float> myPosition;
		CU::Vector3<float> myDirection;
		CU::Matrix44<float> myOrientation;
		CU::Matrix44<float> myOrientationForward;

		float myTime;
		bool myIsFading;
	};
}