#pragma once

#include "EffectListener.h"

struct ID3DX11EffectVariable;
struct ID3DX11EffectScalarVariable;
struct ID3DX11EffectShaderResourceVariable;
struct ID3DX11EffectMatrixVariable;
struct ID3DX11EffectVectorVariable;

namespace Prism
{
	class GBufferData;
	class Scene;

	class SpotLightPass : public EffectListener
	{
	public:
		SpotLightPass(const std::string& aEffect);

		void Render(Scene* aScene, GBufferData& aGBuffer, Texture* aCubemap, bool aTextureProjection);
		void OnEffectLoad() override;

	private:
		void RenderNormal(Scene* aScene, GBufferData& aGBuffer, Texture* aCubemap);
		void RenderTextureProjection(Scene* aScene, GBufferData& aGBuffer, Texture* aCubemap);

		void SendDataToGPU(GBufferData& aGBuffer, Texture* aCubemap, const Camera& aCamera);
		void RemoveDataFromGPU();

		Effect* myEffect = nullptr;
		ID3DX11EffectShaderResourceVariable* myAlbedo = nullptr;
		ID3DX11EffectShaderResourceVariable* myNormal = nullptr;
		ID3DX11EffectShaderResourceVariable* myEmissive = nullptr;
		ID3DX11EffectShaderResourceVariable* myDepth = nullptr;
		ID3DX11EffectShaderResourceVariable* myCubemap = nullptr;

		ID3DX11EffectVariable* mySpotLightVariable;
		ID3DX11EffectMatrixVariable* myInvertedProjection;
		ID3DX11EffectMatrixVariable* myNotInvertedView;
	};
}
