#pragma once

#include <Vector.h>

namespace Prism
{
	class Texture;
	struct DecalInfo
	{
		Texture* myTexture;
		CU::Vector3<float> myPosition;
	};
}