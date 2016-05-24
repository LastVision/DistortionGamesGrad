#pragma once

#include "EngineEnums.h"

#define MAX_NUMBER_OF_SCENES 2

struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct D3D11_VIEWPORT;
namespace Prism
{
	class FullScreenHelper;
	class Scene;
	class SpotLightShadow;
	class Texture;

	struct SceneData;

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void ProcessShadow(SpotLightShadow* aShadowSpotLight, Scene* aScene);
		void Render(Texture* aSource, Texture* aEmissiveTexture, Texture* aDepthStencilTexture, int aEffect);

		void SetRenderTargets(ID3D11RenderTargetView* aRenderTarget, ID3D11DepthStencilView* aDepthBuffer);

	private:
		
		FullScreenHelper* myFullScreenHelper;
		float myClearColor[4];
		

		Texture* myFinalTexture;

		D3D11_VIEWPORT* myShadowViewport;
	};
}

