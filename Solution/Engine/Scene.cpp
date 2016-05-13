#include "stdafx.h"


#include "Camera.h"
#include <Defines.h>
#include <d3dx11effect.h>
#include "DirectionalLight.h"
#include "EngineEnums.h"
#include "Frustum.h"
#include "Instance.h"
#include "PointLight.h"
#include "Scene.h"
#include "SpotLight.h"
#include "InstancingHelper.h"
#include "Texture.h"
#include "Engine.h"

namespace Prism
{
	Scene::Scene()
		: myCamera(nullptr)
		, myInstances(1024)
	{
		myDirectionalLights.Init(4);
		myPointLights.Init(128);
		myAmbientPointLights.Init(128);
		mySpotLights.Init(4);
		mySpotLightsTextureProjection.Init(4);

		myInstancingHelper = new InstancingHelper();
		myInstancingHelper->SetCamera(myCamera);
	}

	Scene::~Scene()
	{
		SAFE_DELETE(myInstancingHelper);
	}

	void Scene::Render()
	{
		UpdateLights();


		for (int i = 0; i < myInstances.Size(); ++i)
		{
			myInstances[i]->Render(*myCamera, *myInstancingHelper);
		}
		
		myInstancingHelper->Render(false);
	}

	void Scene::RenderDepth()
	{
		for (int i = 0; i < myInstances.Size(); ++i)
		{
			myInstances[i]->Render(*myCamera, *myInstancingHelper, true);
		}

		myInstancingHelper->Render(true);
	}

	void Scene::AddInstance(Instance* aInstance)
	{
		myInstances.Add(aInstance);
	}

	void Scene::AddLight(DirectionalLight* aLight)
	{
		myDirectionalLights.Add(aLight);
	}

	void Scene::AddLight(PointLight* aLight)
	{
		if (aLight->GetAmbientOnly() == false)
		{
			myPointLights.Add(aLight);
		}
		else
		{
			myAmbientPointLights.Add(aLight);
		}
	}

	void Scene::AddLight(SpotLight* aLight)
	{
		mySpotLights.Add(aLight);
	}

	void Scene::AddLight(SpotLightTextureProjection* aLight)
	{
		mySpotLightsTextureProjection.Add(aLight);
	}

	void Scene::SetCamera(const Camera& aCamera)
	{
		myCamera = &aCamera;
		myInstancingHelper->SetCamera(myCamera);
	}

	void Scene::RemoveInstance(Instance* aInstance)
	{
		myInstances.RemoveCyclic(aInstance);
	}

	const CU::GrowingArray<PointLight*>& Scene::GetPointLights(bool) const
	{
		return myPointLights;
	}

	const CU::GrowingArray<SpotLight*>& Scene::GetSpotLights(bool) const
	{
		return mySpotLights;
	}

	const CU::GrowingArray<SpotLightTextureProjection*>& Scene::GetSpotLightsTextureProjection(bool) const
	{
		return mySpotLightsTextureProjection;
	}
}