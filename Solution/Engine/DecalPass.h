#pragma once

#include "DecalInfo.h"
#include <GrowingArray.h>

namespace Prism
{
	class Instance;
	class Texture;

	class DecalPass
	{
	public:
		DecalPass();
		~DecalPass();

		void AddDecal(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aDirection, const std::string& aTexturePath);
		void Render(const Camera& aCamera, Texture* aDepthTexture);

	private:
		CU::Matrix44<float> myOrientation;
		Instance* myInstance;

		CU::GrowingArray<DecalInfo> myDecals;
	};
}
