#pragma once

#include "DecalInfo.h"
#include "EffectListener.h"
#include <GrowingArray.h>

struct ID3DX11EffectShaderResourceVariable;

namespace Prism
{
	class Effect;
	class Instance;
	class Texture;
	class GBufferData;

	class DecalPass : public EffectListener
	{
	public:
		DecalPass();
		~DecalPass();

		void AddDecal(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aDirection, const std::string& aTexturePath);

		void Update(float aDelta);
		void Render(const Camera& aCamera, Texture* aDepthTexture, GBufferData* aGBuffer, GBufferData* aGBufferCopy);

	private:
		void OnEffectLoad();

		CU::Matrix44<float> CalculateOrientation(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aDirection);
		void SetGBufferData(GBufferData* aGBuffer, GBufferData* aGBufferCopy);
		void SetDecalVariables(Effect* aEffect, const DecalInfo& aDecal);
		void SetShaderVariables(Effect* aEffect, const CU::Vector3<float>& aDirection);

		CU::Matrix44<float> myOrientation;
		Instance* myInstance;

		CU::GrowingArray<DecalInfo> myDecals;

		Effect* myEffect;
		ID3DX11EffectShaderResourceVariable* myGAlbedo;
		ID3DX11EffectShaderResourceVariable* myGNormal;
		ID3DX11EffectShaderResourceVariable* myGEmissive;
		ID3DX11EffectShaderResourceVariable* myGDepth;

		ID3DX11EffectShaderResourceVariable* myAlbedo;
		ID3DX11EffectShaderResourceVariable* myMetalness;
		ID3DX11EffectShaderResourceVariable* myRoughness;

	};
}
