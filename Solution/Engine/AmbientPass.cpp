#include "stdafx.h"

#include "AmbientPass.h"
#include "Camera.h"
#include <d3dx11effect.h>
#include "Effect.h"
#include "GBufferData.h"
#include "Texture.h"

namespace Prism
{

	AmbientPass::AmbientPass()
	{
		myEffect = EffectContainer::GetInstance()->GetEffect(
			"Data/Resource/Shader/S_effect_deferred_ambient.fx");

		OnEffectLoad();
		myEffect->AddListener(this);

		mySSAORandomTexture = TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/T_random_texture.dds");
	}

	void AmbientPass::SendDataToGPU(GBufferData& aGBuffer, Texture* aCubemap, const Camera& aCamera)
	{
		myAlbedo->SetResource(aGBuffer.myAlbedoTexture->GetShaderView());
		myNormal->SetResource(aGBuffer.myNormalTexture->GetShaderView());
		myEmissive->SetResource(aGBuffer.myEmissiveTexture->GetShaderView());
		myDepth->SetResource(aGBuffer.myDepthTexture->GetShaderView());
		myCubemap->SetResource(aCubemap->GetShaderView());
		mySSAORandomTextureVariable->SetResource(mySSAORandomTexture->GetShaderView());

		myEffect->SetCameraPosition(aCamera.GetOrientation().GetPos());
		myInvertedProjection->SetMatrix(&CU::InverseReal(aCamera.GetProjection()).myMatrix[0]);
		myNotInvertedView->SetMatrix(&aCamera.GetOrientation().myMatrix[0]);
	}

	void AmbientPass::RemoveDataFromGPU()
	{
		myAlbedo->SetResource(nullptr);
		myNormal->SetResource(nullptr);
		myEmissive->SetResource(nullptr);
		myDepth->SetResource(nullptr);
		myCubemap->SetResource(nullptr);
		mySSAORandomTextureVariable->SetResource(nullptr);
	}

	void AmbientPass::OnEffectLoad()
	{
		myAlbedo = myEffect->GetEffect()->GetVariableByName("AlbedoTexture")->AsShaderResource();
		myNormal = myEffect->GetEffect()->GetVariableByName("NormalTexture")->AsShaderResource();
		myEmissive = myEffect->GetEffect()->GetVariableByName("EmissiveTexture")->AsShaderResource();
		myDepth = myEffect->GetEffect()->GetVariableByName("DepthTexture")->AsShaderResource();
		myCubemap = myEffect->GetEffect()->GetVariableByName("CubeMap")->AsShaderResource();
		mySSAORandomTextureVariable = myEffect->GetEffect()->GetVariableByName("RandomTexture")->AsShaderResource();
		myAmbientMultiplier = myEffect->GetEffect()->GetVariableByName("AmbientMultiplier")->AsScalar();

		myInvertedProjection = myEffect->GetEffect()->GetVariableByName("InvertedProjection")->AsMatrix();
		myNotInvertedView = myEffect->GetEffect()->GetVariableByName("NotInvertedView")->AsMatrix();


#ifdef USE_LIGHT
		myAmbientMultiplier->SetFloat(0.05f);
#else
		myAmbientMultiplier->SetFloat(1.f);
#endif
	}
}