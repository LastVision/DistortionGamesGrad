#include "stdafx.h"

#include "Camera.h"
#include <d3dx11effect.h>
#include "DirectionalLight.h"
#include "Effect.h"
#include "GBufferData.h"
#include "PointLight.h"
#include "DirectionalLightPass.h"
#include "Scene.h"


namespace Prism
{

	DirectionalLightPass::DirectionalLightPass()
	{
		myEffect = EffectContainer::GetInstance()->GetEffect(
			"Data/Resource/Shader/S_effect_deferred_directional_light.fx");

		OnEffectLoad();
		myEffect->AddListener(this);
	}

	void DirectionalLightPass::OnEffectLoad()
	{
		myAlbedo = myEffect->GetEffect()->GetVariableByName("AlbedoTexture")->AsShaderResource();
		myNormal = myEffect->GetEffect()->GetVariableByName("NormalTexture")->AsShaderResource();
		myEmissive = myEffect->GetEffect()->GetVariableByName("EmissiveTexture")->AsShaderResource();
		myDepth = myEffect->GetEffect()->GetVariableByName("DepthTexture")->AsShaderResource();
		myCubemap = myEffect->GetEffect()->GetVariableByName("CubeMap")->AsShaderResource();

		myLightVariable = myEffect->GetEffect()->GetVariableByName("DirectionalLight");

		myInvertedProjection = myEffect->GetEffect()->GetVariableByName("InvertedProjection")->AsMatrix();
		myNotInvertedView = myEffect->GetEffect()->GetVariableByName("NotInvertedView")->AsMatrix();
	}

	void DirectionalLightPass::SetLightData(const DirectionalLightData& someData)
	{
		myLightVariable->SetRawValue(&someData, 0, sizeof(DirectionalLightData));
	}

	void DirectionalLightPass::SendDataToGPU(GBufferData& aGBuffer, Texture* aCubemap, const Camera& aCamera)
	{
		myAlbedo->SetResource(aGBuffer.myAlbedoTexture->GetShaderView());
		myNormal->SetResource(aGBuffer.myNormalTexture->GetShaderView());
		myDepth->SetResource(aGBuffer.myDepthTexture->GetShaderView());
		myCubemap->SetResource(aCubemap->GetShaderView());
		myInvertedProjection->SetMatrix(&CU::InverseReal(aCamera.GetProjection()).myMatrix[0]);
		myNotInvertedView->SetMatrix(&aCamera.GetOrientation().myMatrix[0]);
		myEffect->SetCameraPosition(aCamera.GetOrientation().GetPos());
	}

	void DirectionalLightPass::RemoveDataFromGPU()
	{
		myAlbedo->SetResource(nullptr);
		myNormal->SetResource(nullptr);
		myEmissive->SetResource(nullptr);
		myDepth->SetResource(nullptr);
	}
}