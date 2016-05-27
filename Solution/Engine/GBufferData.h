#pragma once

struct ID3D11DepthStencilView;

namespace Prism
{
	class Texture;

	class GBufferData
	{
	public:
		GBufferData();
		~GBufferData();

		void Clear(float* aClearColor);
		void SetAsRenderTarget(Texture* aDepthTexture);
		void SetAsRenderTarget(ID3D11DepthStencilView* aDepthStencil);

		void Copy(const GBufferData& aSource);


		Texture* myAlbedoTexture;
		Texture* myNormalTexture; //METALNESS is stored in W
		Texture* myEmissiveTexture;  //ROUGHNESS is stored in W
		Texture* myDepthTexture;
	};
}