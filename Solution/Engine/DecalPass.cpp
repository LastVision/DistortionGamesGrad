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
		: myOilDecals(16)
		, myLavaDecals(16)
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

	void DecalPass::AddDecal(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aDirection, eDecalType aType)
	{
		switch (aType)
		{
		case OIL:
			AddDecal(aPosition, aDirection, myOilDecals, myDecalTextures[aType]);
			break;
		case LAVA:
			AddDecal(aPosition, aDirection, myLavaDecals, myDecalTextures[aType]);
			break;
		default:
			DL_ASSERT("Invalid Decal-Type");
			break;
		}
	}

	void DecalPass::Update(float aDelta)
	{
		UpdateDecals(myOilDecals, aDelta);
		UpdateDecals(myLavaDecals, aDelta);
	}

	void DecalPass::UpdateDecals(CU::GrowingArray<DecalInfo>& someDecals, float aDelta)
	{
		for (int i = someDecals.Size() - 1; i >= 0; --i)
		{
			DecalInfo& decal = someDecals[i];
			if (decal.myIsFading == true)
			{
				decal.myTime -= aDelta;
				if (decal.myTime <= 0.f)
				{
					someDecals.RemoveNonCyclicAtIndex(i);
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

			//aGBufferCopy->Copy(*aGBuffer);
			RenderDecals(myOilDecals, effect, aGBuffer, aGBufferCopy, aCamera);
			RenderDecals(myLavaDecals, effect, aGBuffer, aGBufferCopy, aCamera);

			myGAlbedo->SetResource(nullptr);
			myGNormal->SetResource(nullptr);
			myGEmissive->SetResource(nullptr);
			myGDepth->SetResource(nullptr);
		}

		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_ENABLED);
		Engine::GetInstance()->SetRasterizeState(eRasterizer::CULL_BACK);
	}

	void DecalPass::OnResize(float aWidth, float aHeight)
	{
		for (int i = myOilDecals.Size() - 1; i >= 0; --i)
		{
			DecalInfo& decal = myOilDecals[i];
			decal.myTextures->myTexture->Resize(aWidth, aHeight);
			decal.myTextures->myEmissive->Resize(aWidth, aHeight);
		}

		for (int i = myLavaDecals.Size() - 1; i >= 0; --i)
		{
			DecalInfo& decal = myLavaDecals[i];
			decal.myTextures->myTexture->Resize(aWidth, aHeight);
			decal.myTextures->myEmissive->Resize(aWidth, aHeight);
		}
	}

	void DecalPass::OnEffectLoad()
	{
		myGAlbedo = myEffect->GetEffect()->GetVariableByName("AlbedoTexture")->AsShaderResource();
		myGNormal = myEffect->GetEffect()->GetVariableByName("NormalTexture")->AsShaderResource();
		myGEmissive = myEffect->GetEffect()->GetVariableByName("EmissiveTexture")->AsShaderResource();
		myGDepth = myEffect->GetEffect()->GetVariableByName("DepthTexture")->AsShaderResource();

		myAlbedo = myEffect->GetEffect()->GetVariableByName("DiffuseTexture")->AsShaderResource();
		myEmissive = myEffect->GetEffect()->GetVariableByName("DecalEmissiveTexture")->AsShaderResource();
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

		myDecalTextures[eDecalType::OIL].Init(16);
		tinyxml2::XMLElement* oilElement = reader.ForceFindFirstChild(root, "Oil");
		for (; oilElement != nullptr; oilElement = reader.FindNextElement(oilElement, "Oil"))
		{
			myDecalTextures[eDecalType::OIL].Add(LoadDecalTextures(reader, oilElement));
		}

		myDecalTextures[eDecalType::LAVA].Init(16);
		tinyxml2::XMLElement* lavaElement = reader.ForceFindFirstChild(root, "Lava");
		for (; lavaElement != nullptr; lavaElement = reader.FindNextElement(lavaElement, "Lava"))
		{
			myDecalTextures[eDecalType::LAVA].Add(LoadDecalTextures(reader, lavaElement));
		}

		reader.CloseDocument();
	}

	Prism::DecalTextures DecalPass::LoadDecalTextures(XMLReader& aReader, tinyxml2::XMLElement* aElement)
	{
		std::string albedo;
		std::string emissive;
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(aElement, "Albedo"), "path", albedo);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(aElement, "Emissive"), "path", emissive);
		DecalTextures textures;
		textures.myTexture = TextureContainer::GetInstance()->GetTexture(albedo);
		textures.myEmissive = TextureContainer::GetInstance()->GetTexture(emissive);

		return textures;
	}

	void DecalPass::AddDecal(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aDirection
		, CU::GrowingArray<DecalInfo>& someDecals, CU::GrowingArray<DecalTextures>& someTextures)
	{
		if (someDecals.Size() > 0)
		{
			DecalInfo& decal = someDecals.GetLast();
			decal.myIsFading = true;
			decal.myTime = myFadeTime;
		}

		if (someDecals.Size() > myMaxDecalCount)
		{
			someDecals.RemoveNonCyclicAtIndex(0);
		}

		int textureIndex = rand() % someTextures.Size();
		float rotation = CU::Math::RandomRange(0.f, 6.28f);

		DecalInfo info;
		info.myPosition = aPosition;
		info.myOrientation = CalculateOrientation(aPosition, aDirection, rotation);
		info.myOrientationForward = CalculateOrientation(aPosition, CU::Vector3<float>(0.f, 0.f, 1.f),rotation);
		//info.myDirection = CalculateOrientation(aPosition, aDirection).GetForward();
		info.myTextures = &someTextures[textureIndex];
		info.myIsFading = false;
		info.myTime = myFadeTime;

		

		someDecals.Add(info);
	}

	void DecalPass::RenderDecals(const CU::GrowingArray<DecalInfo>& someDecals, Effect* aEffect
		, GBufferData* aGBuffer, GBufferData* aGBufferCopy, const Camera& aCamera)
	{
		for each (const DecalInfo& info in someDecals)
		{
			SetDecalVariables(aEffect, info);

			//myOrientation = CalculateOrientation(info.myPosition, info.myDirection);
			myOrientation = info.myOrientation;
			SetShaderVariables(aEffect, info.myOrientation.GetForward());
			SetGBufferData(aGBuffer, aGBufferCopy);
			myInstance->Render(aCamera);

			//myOrientation = CalculateOrientation(info.myPosition, CU::Vector3<float>(0.f, 0.f, 1.f));
			myOrientation = info.myOrientationForward;
			SetShaderVariables(aEffect, info.myOrientationForward.GetForward());
			SetGBufferData(aGBuffer, aGBufferCopy);
			myInstance->Render(aCamera);
		}
	}

	CU::Matrix44<float> DecalPass::CalculateOrientation(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aDirection
		, float aRotation)
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

		orientation = CU::Matrix44<float>::CreateRotateAroundZ(aRotation) * orientation;

		orientation.SetPos(aPosition);
		return orientation;
	}

	void DecalPass::SetGBufferData(GBufferData* aGBuffer, GBufferData* aGBufferCopy)
	{
		aGBufferCopy->Copy(*aGBuffer);
		aGBuffer->SetAlbedoAsRenderTarget(Engine::GetInstance()->GetDepthView());
		myGAlbedo->SetResource(aGBufferCopy->myAlbedoTexture->GetShaderView());
	}

	void DecalPass::SetDecalVariables(Effect* aEffect, const DecalInfo& aDecal)
	{
		myAlbedo->SetResource(aDecal.myTextures->myTexture->GetShaderView());
		myEmissive->SetResource(aDecal.myTextures->myEmissive->GetShaderView());

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