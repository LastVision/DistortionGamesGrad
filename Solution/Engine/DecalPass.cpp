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
#include <XMLReader.h>

namespace Prism
{
	DecalPass::DecalPass()
		: myDecals(16)
		, myDecalTextures(8)
	{
		myEffect = EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_deferred_decal.fx");
		ModelProxy* model = ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Decals/SM_decal_box.fbx", "Data/Resource/Shader/S_effect_deferred_decal.fx");
		myInstance = new Instance(*model, myOrientation);

		LoadFromXML();

		OnEffectLoad();
		myEffect->AddListener(this);
	}


	DecalPass::~DecalPass()
	{
		SAFE_DELETE(myInstance);
	}

	void DecalPass::AddDecal(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aDirection)
	{
		if (myDecals.Size() > 0)
		{
			DecalInfo& decal = myDecals.GetLast();
			decal.myIsFading = true;
			decal.myTime = myFadeTime;
		}

		if (myDecals.Size() > myMaxDecalCount)
		{
			myDecals.RemoveNonCyclicAtIndex(0);
		}

		int textureIndex = rand() % myDecalTextures.Size();

		DecalInfo info;
		info.myPosition = aPosition;
		info.myDirection = aDirection;
		info.myTextures = &myDecalTextures[textureIndex];
		info.myIsFading = false;
		info.myTime = myFadeTime;
		myDecals.Add(info);
	}

	void DecalPass::Update(float aDelta)
	{
		for (int i = myDecals.Size() - 1; i >= 0; --i)
		{
			DecalInfo& decal = myDecals[i];
			if (decal.myIsFading == true)
			{
				decal.myTime -= aDelta;
				if (decal.myTime <= 0.f)
				{
					myDecals.RemoveNonCyclicAtIndex(i);
				}
			}
		}
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
				SetDecalVariables(effect, info);

				myOrientation = CalculateOrientation(info.myPosition, info.myDirection);
				SetShaderVariables(effect, info.myDirection);
				SetGBufferData(aGBuffer, aGBufferCopy);
				myInstance->Render(aCamera);

				myOrientation = CalculateOrientation(info.myPosition, CU::Vector3<float>(0.f, 0.f, 1.f));
				SetShaderVariables(effect, CU::Vector3<float>(0.f, 0.f, 1.f));
				SetGBufferData(aGBuffer, aGBufferCopy);
				myInstance->Render(aCamera);
			}

			myGAlbedo->SetResource(nullptr);
			myGNormal->SetResource(nullptr);
			myGEmissive->SetResource(nullptr);
			myGDepth->SetResource(nullptr);
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
		myNormal = myEffect->GetEffect()->GetVariableByName("DecalNormalTexture")->AsShaderResource();
	}

	void DecalPass::LoadFromXML()
	{
		XMLReader reader;
		reader.OpenDocument("Data/Resource/SET_decals.xml");

		tinyxml2::XMLElement* root = reader.ForceFindFirstChild("root");

		tinyxml2::XMLElement* numberOfDecals = reader.ForceFindFirstChild(root, "MaxNumberOfDecals");
		reader.ForceReadAttribute(numberOfDecals, "value", myMaxDecalCount);

		tinyxml2::XMLElement* fadeTime = reader.ForceFindFirstChild(root, "FadeTime");
		reader.ForceReadAttribute(fadeTime, "value", myFadeTime);

		tinyxml2::XMLElement* decalElement = reader.ForceFindFirstChild(root, "Decal");
		for (; decalElement != nullptr; decalElement = reader.FindNextElement(decalElement, "Decal"))
		{
			std::string albedo;
			std::string normal;
			std::string metalness;
			std::string roughness;

			reader.ForceReadAttribute(reader.ForceFindFirstChild(decalElement, "Albedo"), "path", albedo);
			reader.ForceReadAttribute(reader.ForceFindFirstChild(decalElement, "Normal"), "path", normal);
			reader.ForceReadAttribute(reader.ForceFindFirstChild(decalElement, "Metalness"), "path", metalness);
			reader.ForceReadAttribute(reader.ForceFindFirstChild(decalElement, "Roughness"), "path", roughness);

			DecalTextures textures;
			textures.myTexture = TextureContainer::GetInstance()->GetTexture(albedo);
			textures.myNormalMap = TextureContainer::GetInstance()->GetTexture(normal);
			textures.myMetalness = TextureContainer::GetInstance()->GetTexture(metalness);
			textures.myRoughness = TextureContainer::GetInstance()->GetTexture(roughness);
			myDecalTextures.Add(textures);
		}

		reader.CloseDocument();
	}

	CU::Matrix44<float> DecalPass::CalculateOrientation(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aDirection)
	{
		CU::Matrix44<float> orientation;

		if (aDirection == CU::Vector3<float>(0.f, 0.f, 1.f))
		{
			orientation = CU::Matrix44<float>();
		}
		else
		{
			CU::Vector3<float> forward(aDirection);
			CU::Vector3<float> up = CU::Cross(forward, CU::Vector3<float>(0.f, 0.f, 1.f));
			CU::Normalize(up);
			CU::Vector3<float> right = CU::Cross(forward, up);
			CU::Normalize(up);

			orientation.SetUp(up);
			orientation.SetRight(right);
			orientation.SetForward(forward);
		}

		orientation.SetPos(aPosition);
		return orientation;
	}

	void DecalPass::SetGBufferData(GBufferData* aGBuffer, GBufferData* aGBufferCopy)
	{
		aGBufferCopy->Copy(*aGBuffer);
#ifdef PBL_DECALS
		aGBuffer->SetAsRenderTarget(Engine::GetInstance()->GetDepthView());

		myGAlbedo->SetResource(aGBufferCopy->myAlbedoTexture->GetShaderView());
		myGNormal->SetResource(aGBufferCopy->myNormalTexture->GetShaderView());
		myGEmissive->SetResource(aGBufferCopy->myEmissiveTexture->GetShaderView());
		myGDepth->SetResource(aGBufferCopy->myDepthTexture->GetShaderView());
#else
		aGBuffer->SetAlbedoAsRenderTarget(Engine::GetInstance()->GetDepthView());
		myGAlbedo->SetResource(aGBufferCopy->myAlbedoTexture->GetShaderView());
#endif
	}

	void DecalPass::SetDecalVariables(Effect* aEffect, const DecalInfo& aDecal)
	{
		myAlbedo->SetResource(aDecal.myTextures->myTexture->GetShaderView());

#ifdef PBL_DECALS
		myMetalness->SetResource(aDecal.myTextures->myMetalness->GetShaderView());
		myRoughness->SetResource(aDecal.myTextures->myRoughness->GetShaderView());
		myNormal->SetResource(aDecal.myTextures->myNormalMap->GetShaderView());
#endif

		float alpha = min(1.f, aDecal.myTime);
		if (alpha < 1.f)
		{
			int apa = 5;
		}
		aEffect->SetAlpha(alpha);
	}

	void DecalPass::SetShaderVariables(Effect* aEffect, const CU::Vector3<float>& aDirection)
	{
		aEffect->SetWorldMatrixInverted(CU::InverseSimple(myOrientation));
		aEffect->SetDecalDirection(aDirection);
	}

}