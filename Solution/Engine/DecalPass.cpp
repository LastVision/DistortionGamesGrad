#include "stdafx.h"

#include "Camera.h"
#include "DecalPass.h"
#include "Instance.h"
#include "ModelLoader.h"
#include "Texture.h"

namespace Prism
{
	DecalPass::DecalPass()
		: myDecals(16)
	{
		ModelProxy* model = ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Decals/SM_decal_box.fbx", "Data/Resource/Shader/S_effect_deferred_decal.fx");
		//ModelProxy* model = ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Decals/SM_decal_box_large.fbx", "Data/Resource/Shader/S_effect_deferred_decal.fx");
		myInstance = new Instance(*model, myOrientation);

		TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/Decal/T_decal_test.dds");
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
		myDecals.Add(info);
	}

	void DecalPass::Render(const Camera& aCamera, Texture* aDepthTexture)
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
				effect->SetTexture(info.myTexture);
				effect->SetDecalDirection(info.myDirection);
				myInstance->Render(aCamera);
			}
			

		}

		Engine::GetInstance()->SetDepthBufferState(eDepthStencil::Z_ENABLED);
		Engine::GetInstance()->SetRasterizeState(eRasterizer::CULL_BACK);
	}

}