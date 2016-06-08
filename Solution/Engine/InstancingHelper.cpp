#include "stdafx.h"

#include "Camera.h"
#include "InstancingHelper.h"
#include "Effect.h"
#include <d3dx11effect.h>

namespace Prism
{
	InstancingHelper::InstancingHelper()
		: myCamera(nullptr)
		, myModels(128)
	{
	}

	void InstancingHelper::SetCamera(const Camera* aCamera)
	{
		myCamera = aCamera;
	}

	void InstancingHelper::AddModel(Model* aModel, const CU::Matrix44<float>& aWorldMatrix
		, const CU::Vector3<float>& aScale, float aHeight)
	{
		if (myModels.Find(aModel) == myModels.FoundNone)
		{
			myModels.Add(aModel);
		}

		aModel->myMatrices.Add(aWorldMatrix);
		aModel->myScales.Add(aScale);
		//aModel->myHeights.Add(aHeight);
	}

	void InstancingHelper::Render(bool aIsOnlyDepth)
	{
		DL_ASSERT_EXP(myCamera != nullptr, "Tried to render without a camera");

		Effect* oldEffect = nullptr;

		for each (Model* model in myModels)
		{
			if (model->myMatrices.Size() > 0)
			{
				Effect* currEffect = model->GetEffect();
				if (currEffect != oldEffect)
				{
					oldEffect = currEffect;

					currEffect->SetViewProjectionMatrix(myCamera->GetViewProjection());
					currEffect->SetScaleVector({ 1.f, 1.f, 1.f });
					currEffect->SetCameraPosition(myCamera->GetOrientation().GetPos());
				}

				RenderModel(model, currEffect, aIsOnlyDepth);

				
				model->myMatrices.RemoveAll();
				model->myScales.RemoveAll();
			}
		}

		DEBUG_PRINT(myDrawCalls);
		myDrawCalls = 0;

		DEBUG_PRINT(myRenderedCount);
		myRenderedCount = 0;
	}

	void InstancingHelper::RenderModel(Model* aModel, Effect* aEffect, bool aIsOnlyDepth)
	{
		if (aModel->SetGPUState(aModel->myMatrices, aModel->myScales))
		{
			D3DX11_TECHNIQUE_DESC techDesc;
			ID3DX11EffectTechnique* tech;

			myRenderedCount += aModel->myMatrices.Size();
			++myDrawCalls;
			if (aIsOnlyDepth == true)
			{
				tech = aEffect->GetTechnique(aModel->GetTechniqueNameDepthOnly());
			}
			else
			{
				tech = aEffect->GetTechnique(aModel->GetTechniqueName());
			}

			tech->GetDesc(&techDesc);

			if (tech->IsValid() == false)
			{
				tech = aEffect->GetTechnique();
				DL_ASSERT("INVALID TECHNIQUE IN INSTANCEHELPER::RENDER:");
			}

			for (UINT j = 0; j < techDesc.Passes; ++j)
			{
				ID3D11DeviceContext* context = Engine::GetInstance()->GetContex();
				tech->GetPassByIndex(j)->Apply(0, context);
				context->DrawIndexedInstanced(aModel->GetIndexCount(), aModel->myMatrices.Size()
					, 0, aModel->GetVertexStart(), 0);

			}
			aModel->DeActivateSurfaces();
		}

		for (int i = 0; i < aModel->GetChildren().Size(); ++i)
		{
			RenderModel(aModel->GetChildren()[i], aEffect, aIsOnlyDepth);
		}
	}
}

