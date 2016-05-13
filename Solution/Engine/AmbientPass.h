#pragma once

#include "EffectListener.h"

struct ID3DX11EffectVariable;
struct ID3DX11EffectScalarVariable;
struct ID3DX11EffectShaderResourceVariable;
struct ID3DX11EffectMatrixVariable;
struct ID3DX11EffectVectorVariable;


namespace Prism
{
	class Camera;
	class GBufferData;
	class Texture;
	class AmbientPass : public EffectListener
	{
	public:
		AmbientPass();

		void SendDataToGPU(GBufferData& aGBuffer, Texture* aCubemap, const Camera& aCamera);
		void RemoveDataFromGPU();
		void OnEffectLoad() override;

		Effect* myEffect = nullptr;

	private:
		ID3DX11EffectShaderResourceVariable* myAlbedo = nullptr;
		ID3DX11EffectShaderResourceVariable* myNormal = nullptr;
		ID3DX11EffectShaderResourceVariable* myEmissive = nullptr;
		ID3DX11EffectShaderResourceVariable* myDepth = nullptr;
		ID3DX11EffectShaderResourceVariable* myCubemap = nullptr;

		ID3DX11EffectShaderResourceVariable* mySSAORandomTextureVariable = nullptr;
		ID3DX11EffectScalarVariable* myAmbientMultiplier = nullptr;
		ID3DX11EffectMatrixVariable* myInvertedProjection;
		ID3DX11EffectMatrixVariable* myNotInvertedView;

		Texture* mySSAORandomTexture;
	};
}
