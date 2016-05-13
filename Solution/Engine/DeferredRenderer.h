#pragma once
#include "BaseModel.h"
#include "DeferredPasses.h"

#include "ShadowPass.h"

struct D3D11_VIEWPORT;

namespace Prism
{
	class AmbientPass;
	class PointLightPass;
	class SpotLightPass;
	class GBufferData;

	class Effect;
	class Scene;
	class Texture;
	class SpriteProxy;
	class SpotLightShadow;

	class EmitterManager;
	class DeferredRenderer : public BaseModel
	{
	public:
		DeferredRenderer();
		~DeferredRenderer();

		void Render(Scene* aScene, Prism::SpriteProxy* aBackground, Prism::SpotLightShadow* aShadowLight, EmitterManager* aParticleEmitterManager);
		void RenderShadows(Prism::SpotLightShadow* aShadowLight, const Prism::Camera* aCamera);
		void OnResize(float aWidth, float aHeight);

		void SetCubeMap(const std::string& aFilePath);

		Texture* GetFinishedTexture();
		Texture* GetEmissiveTexture();
		Texture* GetDepthStencilTexture();

	private:
		void InitFullscreenQuad();
		void CreateVertices();
		void ActivateBuffers();
		void Render(Effect* aEffect, const std::string& aTechnique = "Render");
		void RenderDeferred(Scene* aScene);
		void RenderAmbientPass(Scene* aScene);

		void SetupShadowData();

		Texture* myDepthStencilTexture;
		Texture* myCubemap;
		Texture* myFinishedSceneTexture;
		Texture* myFinishedTexture;

		RenderToScreenData myRenderToScreenData;
		AmbientPass* myAmbientPass;
		PointLightPass* myPointLightPass;
		SpotLightPass* mySpotLightPass;
		SpotLightPass* mySpotLightTextureProjectionPass;
		ShadowPass myShadowPass;
		GBufferData* myGBufferData;
		D3D11_VIEWPORT* myViewPort;

		float myClearColor[4];
	};
}