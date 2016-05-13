#include "stdafx.h"

#include "Camera.h"
#include <d3dx11effect.h>
#include "Effect.h"
#include "GBufferData.h"
#include "Scene.h"
#include "SpotLight.h"
#include "SpotLightPass.h"
#include "SpotLightTextureProjection.h"

namespace Prism
{

	SpotLightPass::SpotLightPass(const std::string& aEffect)
	{
		myEffect = EffectContainer::GetInstance()->GetEffect(aEffect);

		OnEffectLoad();
		myEffect->AddListener(this);
	}

	void SpotLightPass::Render(Scene* aScene, GBufferData& aGBuffer, Texture* aCubemap, bool aTextureProjection)
	{
		if (aTextureProjection == false)
		{
			RenderNormal(aScene, aGBuffer, aCubemap);
		}
		else
		{
			RenderTextureProjection(aScene, aGBuffer, aCubemap);
		}
	}

	void SpotLightPass::OnEffectLoad()
	{
		myAlbedo = myEffect->GetEffect()->GetVariableByName("AlbedoTexture")->AsShaderResource();
		myNormal = myEffect->GetEffect()->GetVariableByName("NormalTexture")->AsShaderResource();
		myEmissive = myEffect->GetEffect()->GetVariableByName("EmissiveTexture")->AsShaderResource();
		myDepth = myEffect->GetEffect()->GetVariableByName("DepthTexture")->AsShaderResource();
		myCubemap = myEffect->GetEffect()->GetVariableByName("CubeMap")->AsShaderResource();

		mySpotLightVariable = myEffect->GetEffect()->GetVariableByName("SpotLights");

		myInvertedProjection = myEffect->GetEffect()->GetVariableByName("InvertedProjection")->AsMatrix();
		myNotInvertedView = myEffect->GetEffect()->GetVariableByName("NotInvertedView")->AsMatrix();
	}

	void SpotLightPass::RenderNormal(Scene* aScene, GBufferData& aGBuffer, Texture* aCubemap)
	{
		const Camera& camera = *aScene->GetCamera();

		Engine::GetInstance()->RestoreViewPort();
		SendDataToGPU(aGBuffer, aCubemap, camera);


		const CU::GrowingArray<SpotLight*>& lights = aScene->GetSpotLights(false);

		Engine::GetInstance()->SetRasterizeState(eRasterizer::NO_CULLING);
		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::READ_NO_WRITE);
		for (int i = 0; i < lights.Size(); ++i)
		{
			mySpotLightVariable->SetRawValue(&lights[i]->GetLightData(), 0, sizeof(SpotLightData));
			lights[i]->Render(camera);
		}
		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_ENABLED);
		Engine::GetInstance()->SetRasterizeState(eRasterizer::CULL_BACK);

		RemoveDataFromGPU();
	}

	void SpotLightPass::RenderTextureProjection(Scene* aScene, GBufferData& aGBuffer, Texture* aCubemap)
	{
		const Camera& camera = *aScene->GetCamera();

		Engine::GetInstance()->RestoreViewPort();
		SendDataToGPU(aGBuffer, aCubemap, camera);

		const CU::GrowingArray<SpotLightTextureProjection*>& lights = aScene->GetSpotLightsTextureProjection(false);

		Engine::GetInstance()->SetRasterizeState(eRasterizer::NO_CULLING);

		for (int i = 0; i < lights.Size(); ++i)
		{
			mySpotLightVariable->SetRawValue(&lights[i]->GetLightData(), 0, sizeof(SpotLightData));

			lights[i]->Render(camera);
		}

		Engine::GetInstance()->SetRasterizeState(eRasterizer::CULL_BACK);

		RemoveDataFromGPU();
	}

	void SpotLightPass::SendDataToGPU(GBufferData& aGBuffer, Texture* aCubemap, const Camera& aCamera)
	{
		myAlbedo->SetResource(aGBuffer.myAlbedoTexture->GetShaderView());
		myNormal->SetResource(aGBuffer.myNormalTexture->GetShaderView());
		myDepth->SetResource(aGBuffer.myDepthTexture->GetShaderView());
		myCubemap->SetResource(aCubemap->GetShaderView());
		myInvertedProjection->SetMatrix(&CU::InverseReal(aCamera.GetProjection()).myMatrix[0]);
		myNotInvertedView->SetMatrix(&aCamera.GetOrientation().myMatrix[0]);
	}

	void SpotLightPass::RemoveDataFromGPU()
	{
		myAlbedo->SetResource(nullptr);
		myNormal->SetResource(nullptr);
		myEmissive->SetResource(nullptr);
		myDepth->SetResource(nullptr);
	}

}