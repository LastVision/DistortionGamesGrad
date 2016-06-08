#pragma once
#include <Defines.h>
#include <GrowingArray.h>
#include "LightStructs.h"
#include <StaticArray.h>

namespace Prism
{
	class Camera;
	class DirectionalLight;
	class Instance;
	class PointLight;
	class Room;
	class RoomManager;
	class SpotLight;
	class SpotLightShadow;
	class InstancingHelper;
	class Texture;
	class SpotLightTextureProjection;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void Render();
		void RenderDepth();

		void RenderStatic();
		void RenderDynamic();
		void RenderSea(Texture* aDepthTexture);
		void DebugPrint();

		void AddInstance(Instance* aInstance, bool aDynamic);
		void SetSea(Instance* aInstance, bool aDynamic);
		void AddLight(DirectionalLight* aLight);
		void AddLight(PointLight* aLight);
		void AddLight(SpotLight* aLight);
		void AddLight(SpotLightTextureProjection* aLight);

		void RemoveInstance(Instance* aInstance);

		void SetCamera(const Camera& aCamera);
		const Camera* GetCamera() const;

		const CU::GrowingArray<PointLight*>& GetPointLights(bool aUseRoomManager) const;
		const CU::GrowingArray<SpotLight*>& GetSpotLights(bool aUseRoomManager) const;
		const CU::GrowingArray<SpotLightTextureProjection*>& GetSpotLightsTextureProjection(bool aUseRoomManager) const;
		const CU::GrowingArray<DirectionalLight*>& GetDirectionalLights() const;

		void SortInstances();

	private:
		void operator=(Scene&) = delete;

		CU::GrowingArray<DirectionalLight*> myDirectionalLights;
		CU::GrowingArray<PointLight*> myPointLights;
		CU::GrowingArray<PointLight*> myAmbientPointLights;
		CU::GrowingArray<SpotLight*> mySpotLights;
		CU::GrowingArray<SpotLightTextureProjection*> mySpotLightsTextureProjection;
		CU::GrowingArray<Instance*> myStaticInstances;
		CU::GrowingArray<Instance*> myDynamicInstances;
		Instance* mySea;
		
		InstancingHelper* myInstancingHelper;

		const Camera* myCamera;
	};

	inline const Camera* Scene::GetCamera() const
	{
		return myCamera;
	}
}