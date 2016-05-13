#pragma once

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

		Texture* myAlbedoTexture;
		Texture* myNormalTexture; //METALNESS is stored in W
		Texture* myEmissiveTexture;  //ROUGHNESS is stored in W
		Texture* myDepthTexture;
	};
}