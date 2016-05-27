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
		, myDynamicInstances(1024)
		, myStaticInstances(1024)
		, mySea(nullptr)
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
		RenderStatic();
		RenderDynamic();
	}

	void Scene::RenderDepth()
	{
		for (int i = 0; i < myStaticInstances.Size(); ++i)
		{
			myStaticInstances[i]->RenderInstanced(*myCamera, *myInstancingHelper, true);
		}

		myInstancingHelper->Render(true);

		for (int i = 0; i < myDynamicInstances.Size(); ++i)
		{
			myDynamicInstances[i]->RenderInstanced(*myCamera, *myInstancingHelper, true);
		}

		myInstancingHelper->Render(true);
	}

	void Scene::RenderStatic()
	{
		for (int i = 0; i < myStaticInstances.Size(); ++i)
		{
			myStaticInstances[i]->RenderInstanced(*myCamera, *myInstancingHelper);
		}

		myInstancingHelper->Render(false);
	}

	void Scene::RenderDynamic()
	{
		for (int i = 0; i < myDynamicInstances.Size(); ++i)
		{
			myDynamicInstances[i]->RenderInstanced(*myCamera, *myInstancingHelper);
		}

		myInstancingHelper->Render(false);
	}

	void Scene::RenderSea(Texture* aDepthTexture)
	{
		if (mySea->GetModel().IsLoaded() == true)
		{
			mySea->GetModel().GetEffect()->SetDepthTexture(aDepthTexture);
		}
		mySea->RenderInstanced(*myCamera, *myInstancingHelper);
		myInstancingHelper->Render(false);
	}

	void Scene::AddInstance(Instance* aInstance, bool aDynamic)
	{
		if (aDynamic == true)
		{
			myDynamicInstances.Add(aInstance);
		}
		else
		{
			myStaticInstances.Add(aInstance);
		}
	}

	void Scene::SetSea(Instance* aInstance, bool aDynamic)
	{
		mySea = aInstance;
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
		int index = myDynamicInstances.Find(aInstance);
		if (index != myDynamicInstances.FoundNone)
		{
			myDynamicInstances.RemoveCyclicAtIndex(index);
		}
		else
		{
			index = myStaticInstances.Find(aInstance);
			DL_ASSERT_EXP(index != myStaticInstances.FoundNone, "Couldnt find instance to remove");
			myStaticInstances.RemoveCyclicAtIndex(index);
		}
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

	const CU::GrowingArray<DirectionalLight*>& Scene::GetDirectionalLights() const
	{
		return myDirectionalLights;
	}

}