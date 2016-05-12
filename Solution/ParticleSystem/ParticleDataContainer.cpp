#include "stdafx.h"
#include "ParticleDataContainer.h"

#include "ParticleEmitterData.h"
#include <d3dx11effect.h>

#include "Camera.h"
#include "Effect.h"
#include "Engine.h"


namespace Prism
{
	ParticleDataContainer* ParticleDataContainer::myInstance = nullptr;

	ParticleDataContainer* ParticleDataContainer::GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new ParticleDataContainer();
		}

		return myInstance;
	}

	void ParticleDataContainer::Destroy()
	{
		SAFE_DELETE(myInstance);
	}

	ParticleDataContainer::~ParticleDataContainer()
	{
		for (auto it = myParticleData.begin(); it != myParticleData.end(); ++it)
		{
			delete it->second;
			it->second = nullptr;
		}
	}

	ParticleEmitterData* ParticleDataContainer::GetParticleData(const std::string& aFilePath)
	{
		auto it = myParticleData.find(aFilePath);
		if (it == myParticleData.end())
		{
			LoadParticleData(aFilePath);
		}

		return myParticleData[aFilePath];
	}


	void ParticleDataContainer::SetGPUData(const Camera& aCamera)
	{
		for (auto it = myParticleData.begin(); it != myParticleData.end(); ++it)
		{
			ParticleEmitterData* tempData = it->second;
			tempData->myEffect->SetViewMatrix(CU::InverseSimple(aCamera.GetOrientation()));
			tempData->myEffect->SetProjectionMatrix(aCamera.GetProjection());

			Engine::GetInstance()->GetContex()->IASetInputLayout(tempData->myInputLayout);
			Engine::GetInstance()->GetContex()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

			tempData->myEffect->GetTechnique("Render")->GetDesc(tempData->myTechniqueDesc);
		}
	}

	void ParticleDataContainer::LoadParticleData(const std::string& aFilePath)
	{
		ParticleEmitterData* newData = new ParticleEmitterData();
		newData->LoadDataFile(aFilePath);
		DL_ASSERT_EXP(newData != nullptr, "Failed to load data. newData became nullptr.");
		myParticleData[aFilePath] = newData;
	}
}

 