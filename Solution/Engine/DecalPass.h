#pragma once

#include "DecalInfo.h"
#include "EffectListener.h"
#include <GrowingArray.h>
#include <XMLReader.h>

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

		void AddDecal(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aDirection, eDecalType aType);

		void Update(float aDelta);
		void Render(const Camera& aCamera, Texture* aDepthTexture, GBufferData* aGBuffer, GBufferData* aGBufferCopy);
		void OnResize(float aWidth, float aHeight);

	private:
		void OnEffectLoad();
		void LoadFromXML();
		DecalTextures LoadDecalTextures(XMLReader& aReader, tinyxml2::XMLElement* aElement);
		void AddDecal(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aDirection
			, CU::GrowingArray<DecalInfo>& someDecals, CU::GrowingArray<DecalTextures>& someTextures);

		void UpdateDecals(CU::GrowingArray<DecalInfo>& someDecals, float aDelta);
		void RenderDecals(const CU::GrowingArray<DecalInfo>& someDecals, Effect* aEffect
			, GBufferData* aGBuffer, GBufferData* aGBufferCopy, const Camera& aCamera);
		CU::Matrix44<float> CalculateOrientation(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aDirection
			, float aRotation);
		void SetGBufferData(GBufferData* aGBuffer, GBufferData* aGBufferCopy);
		void SetDecalVariables(Effect* aEffect, const DecalInfo& aDecal);
		void SetShaderVariables(Effect* aEffect, const CU::Vector3<float>& aDirection);

		CU::Matrix44<float> myOrientation;
		Instance* myInstance;

		CU::StaticArray<CU::GrowingArray<DecalTextures>, eDecalType::_DECALCOUNT> myDecalTextures;
		CU::GrowingArray<DecalInfo> myOilDecals;
		CU::GrowingArray<DecalInfo> myLavaDecals;
		float myFadeTime;
		int myMaxDecalCount;

		Effect* myEffect;

		ID3DX11EffectShaderResourceVariable* myAlbedo;
		ID3DX11EffectShaderResourceVariable* myEmissive;


		ID3DX11EffectShaderResourceVariable* myGAlbedo;
		ID3DX11EffectShaderResourceVariable* myGNormal;
		ID3DX11EffectShaderResourceVariable* myGEmissive;
		ID3DX11EffectShaderResourceVariable* myGDepth;

		
		ID3DX11EffectShaderResourceVariable* myMetalness;
		ID3DX11EffectShaderResourceVariable* myRoughness;
		ID3DX11EffectShaderResourceVariable* myNormal;

	};
}
