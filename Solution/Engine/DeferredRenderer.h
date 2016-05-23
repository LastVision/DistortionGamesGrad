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
	class ParticlePass;

	class DecalPass;
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

		void AddDecal(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aDirection, const std::string& aPath);

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

		void RenderParticles(EmitterManager* aParticleEmitterManager);

		Texture* myDepthStencilTexture;
		Texture* myCubemap;
		Texture* myFinishedSceneTexture;
		Texture* myFinishedTexture;
		Texture* myParticleTexture;

		ParticlePass* myParticlePass;
		RenderToScreenData myRenderToScreenData;
		AmbientPass* myAmbientPass;
		PointLightPass* myPointLightPass;
		SpotLightPass* mySpotLightPass;
		SpotLightPass* mySpotLightTextureProjectionPass;
		ShadowPass myShadowPass;
		GBufferData* myGBufferData;
		D3D11_VIEWPORT* myViewPort;

		DecalPass* myDecal;

		float myClearColor[4];
	};
}