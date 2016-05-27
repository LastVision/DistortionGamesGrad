#include "stdafx.h"

#include "Camera.h"
#include <d3dx11effect.h>
#include "DecalPass.h"
#include "Effect.h"
#include "EffectContainer.h"
#include "GBufferData.h"
#include "Instance.h"
#include "ModelLoader.h"
#include "Texture.h"

namespace Prism
{
	DecalPass::DecalPass()
		: myDecals(16)
	{
		myEffect = EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_deferred_decal.fx");
		ModelProxy* model = ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Decals/SM_decal_box.fbx", "Data/Resource/Shader/S_effect_deferred_decal.fx");
		//ModelProxy* model = ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Decals/SM_decal_box_large.fbx", "Data/Resource/Shader/S_effect_deferred_decal.fx");
		myInstance = new Instance(*model, myOrientation);

		TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/Decal/T_decal_test.dds");
		//TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/Decal/T_decal_test_metalness.dds");
		//TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/Decal/T_decal_test_roughness.dds");

		OnEffectLoad();
		myEffect->AddListener(this);
	}


	DecalPass::~DecalPass()
	{
		SAFE_DELETE(myInstance);
	}

	void DecalPass::AddDecal(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aDirection, const std::string& aTexturePath)
	{
		DecalInfo info;
		info.myPosition = aPosition;
		info.myDirection = aDirection;
		info.myTexture = TextureContainer::GetInstance()->GetTexture(aTexturePath);
		//info.myMetalness = TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/Decal/T_decal_test_metalness.dds");
		//info.myRoughness = TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/Decal/T_decal_test_roughness.dds");
		myDecals.Add(info);
	}

	void DecalPass::Render(const Camera& aCamera, Texture* aDepthTexture, GBufferData* aGBuffer, GBufferData* aGBufferCopy)
	{
		Engine::GetInstance()->SetRasterizeState(eRasterizer::NO_CULLING);
		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_DISABLED);
		
		if (myInstance->GetModel().IsLoaded())
		{
			Effect* effect = myInstance->GetModel().GetEffect();
			
			static CU::Matrix44<float> lastOrientation = aCamera.GetOrientation();

			effect->SetViewMatrix(CU::InverseSimple(lastOrientation));
			effect->SetViewMatrixNotInverted(lastOrientation);
			effect->SetProjectionMatrix(aCamera.GetProjection());
			effect->SetProjectionMatrixInverted(CU::InverseReal(aCamera.GetProjection()));
			effect->SetDepthTexture(aDepthTexture);

			lastOrientation = aCamera.GetOrientation();

			for each (const DecalInfo& info in myDecals)
			{
				if (info.myDirection == CU::Vector3<float>(0.f, 0.f, 1.f))
				{
					myOrientation = CU::Matrix44<float>();
				}
				else
				{
					CU::Vector3<float> forward(info.myDirection);
					CU::Vector3<float> up = CU::Cross(forward, CU::Vector3<float>(0.f, 0.f, 1.f));
					CU::Normalize(up);
					CU::Vector3<float> right = CU::Cross(forward, up);
					CU::Normalize(up);

					myOrientation.SetUp(up);
					myOrientation.SetRight(right);
					myOrientation.SetForward(forward);
				}

				myOrientation.SetPos(info.myPosition);
				effect->SetWorldMatrixInverted(CU::InverseSimple(myOrientation));
				effect->SetDecalDirection(info.myDirection);

				myAlbedo->SetResource(info.myTexture->GetShaderView());

				//myMetalness->SetResource(info.myMetalness->GetShaderView());
				//myRoughness->SetResource(info.myRoughness->GetShaderView());

				aGBufferCopy->Copy(*aGBuffer);
				//aGBuffer->SetAsRenderTarget(Engine::GetInstance()->GetDepthView());
				ID3D11RenderTargetView* target = aGBuffer->myAlbedoTexture->GetRenderTargetView();
				Engine::GetInstance()->GetContex()->OMSetRenderTargets(1, &target
					, Engine::GetInstance()->GetDepthView());
				
				myGAlbedo->SetResource(aGBufferCopy->myAlbedoTexture->GetShaderView());
				//myGNormal->SetResource(aGBufferCopy->myNormalTexture->GetShaderView());
				//myGEmissive->SetResource(aGBufferCopy->myEmissiveTexture->GetShaderView());
				//myGDepth->SetResource(aGBufferCopy->myDepthTexture->GetShaderView());

				myInstance->Render(aCamera);
			}

			myGAlbedo->SetResource(nullptr);
			//myGNormal->SetResource(nullptr);
			//myGEmissive->SetResource(nullptr);
			//myGDepth->SetResource(nullptr);
		}

		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_ENABLED);
		Engine::GetInstance()->SetRasterizeState(eRasterizer::CULL_BACK);
	}

	void DecalPass::OnEffectLoad()
	{
		myGAlbedo = myEffect->GetEffect()->GetVariableByName("AlbedoTexture")->AsShaderResource();
		myGNormal = myEffect->GetEffect()->GetVariableByName("NormalTexture")->AsShaderResource();
		myGEmissive = myEffect->GetEffect()->GetVariableByName("EmissiveTexture")->AsShaderResource();
		myGDepth = myEffect->GetEffect()->GetVariableByName("DepthTexture")->AsShaderResource();

		myAlbedo = myEffect->GetEffect()->GetVariableByName("DiffuseTexture")->AsShaderResource();
		myMetalness = myEffect->GetEffect()->GetVariableByName("MetalnessTexture")->AsShaderResource();
		myRoughness = myEffect->GetEffect()->GetVariableByName("RoughnessTexture")->AsShaderResource();

	}
}