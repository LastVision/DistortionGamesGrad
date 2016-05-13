#include "stdafx.h"

#include "Camera.h"
#include <d3dx11effect.h>
#include "Effect.h"
#include "GBufferData.h"
#include "PointLight.h"
#include "PointLightPass.h"
#include "Scene.h"


namespace Prism
{

	PointLightPass::PointLightPass()
	{
		myEffect = EffectContainer::GetInstance()->GetEffect(
			"Data/Resource/Shader/S_effect_deferred_light_mesh_point.fx");

		OnEffectLoad();
		myEffect->AddListener(this);
	}

	void PointLightPass::Render(Scene* aScene, GBufferData& aGBuffer, Texture* aCubemap)
	{
		const Camera& camera = *aScene->GetCamera();

		Engine::GetInstance()->RestoreViewPort();

		SendDataToGPU(aGBuffer, aCubemap, camera);

		const CU::GrowingArray<PointLight*>& lights = aScene->GetPointLights(false);

		Engine::GetInstance()->SetRasterizeState(eRasterizer::NO_CULLING);
		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::READ_NO_WRITE);
		for (int i = 0; i < lights.Size(); ++i)
		{
			myPointLightVariable->SetRawValue(&lights[i]->GetLightData(), 0, sizeof(PointLightData));
			lights[i]->Render(camera);
		}
		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_ENABLED);
		Engine::GetInstance()->SetRasterizeState(eRasterizer::CULL_BACK);

		RemoveDataFromGPU();
	}

	void PointLightPass::OnEffectLoad()
	{
		myAlbedo = myEffect->GetEffect()->GetVariableByName("AlbedoTexture")->AsShaderResource();
		myNormal = myEffect->GetEffect()->GetVariableByName("NormalTexture")->AsShaderResource();
		myEmissive = myEffect->GetEffect()->GetVariableByName("EmissiveTexture")->AsShaderResource();
		myDepth = myEffect->GetEffect()->GetVariableByName("DepthTexture")->AsShaderResource();
		myCubemap = myEffect->GetEffect()->GetVariableByName("CubeMap")->AsShaderResource();

		myPointLightVariable = myEffect->GetEffect()->GetVariableByName("PointLights");

		myInvertedProjection = myEffect->GetEffect()->GetVariableByName("InvertedProjection")->AsMatrix();
		myNotInvertedView = myEffect->GetEffect()->GetVariableByName("NotInvertedView")->AsMatrix();
	}

	void PointLightPass::SendDataToGPU(GBufferData& aGBuffer, Texture* aCubemap, const Camera& aCamera)
	{
		myAlbedo->SetResource(aGBuffer.myAlbedoTexture->GetShaderView());
		myNormal->SetResource(aGBuffer.myNormalTexture->GetShaderView());
		myDepth->SetResource(aGBuffer.myDepthTexture->GetShaderView());
		myCubemap->SetResource(aCubemap->GetShaderView());
		myInvertedProjection->SetMatrix(&CU::InverseReal(aCamera.GetProjection()).myMatrix[0]);
		myNotInvertedView->SetMatrix(&aCamera.GetOrientation().myMatrix[0]);
	}

	void PointLightPass::RemoveDataFromGPU()
	{
		myAlbedo->SetResource(nullptr);
		myNormal->SetResource(nullptr);
		myEmissive->SetResource(nullptr);
		myDepth->SetResource(nullptr);
	}

}