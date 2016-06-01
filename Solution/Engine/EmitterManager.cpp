#include "stdafx.h"

#include "EmitterManager.h"
#include "../PostMaster/EmitterMessage.h"
#include "ModelLoader.h"
#include "../ParticleSystem/ParticleDataContainer.h"
#include "../ParticleSystem/ParticleEmitterInstance.h"
#include "../PostMaster/PostMaster.h"
#include <XMLReader.h>
#include <TimerManager.h>
#include "CommonHelper.h"
#include "Camera.h"
#include <InputWrapper.h>
#include "../Entity/Entity.h"
#define FINISHED 0
#define UNFINISHED 1

namespace Prism
{
	EmitterManager::EmitterManager()
		: myEmitterList(64)
	{
		PostMaster* postMaster = PostMaster::GetInstance();
		postMaster->Subscribe(this, eMessageType::PARTICLE);
		Prism::ModelLoader::GetInstance()->Pause();
		ReadListOfLists("Data/Resource/Particle/LI_emitter_lists.xml");

		for (auto it = myEmitters.begin(); it != myEmitters.end(); ++it)
		{
			myEmitterList.Add(it->second);
		}
		Prism::ModelLoader::GetInstance()->UnPause();
		short meter = 60;
		myCullDistance = meter * meter;
	}

	EmitterManager::~EmitterManager()
	{
		PostMaster* postMaster = PostMaster::GetInstance();
		postMaster->UnSubscribe(this, 0);
		myEmitterList.RemoveAll();
		for (auto it = myEmitters.begin(); it != myEmitters.end(); ++it)
		{
			delete it->second;
			it->second = nullptr;
		}
	}

	void EmitterManager::Initiate(Prism::Camera* aCamera)
	{
		myCamera = aCamera;
	}

	void EmitterManager::UpdateEmitters(float aDeltaTime)
	{

		for (int i = 0; i < myEmitterList.Size(); ++i)
		{
			EmitterData* emitterData = myEmitterList[i];
			if (emitterData->myTypeIsActive == false)
			{
				continue;
			}
			for (int k = 0; k < PREALLOCATED_EMITTERGROUP; k++)
			{
				if (emitterData->myFinishedGroups[k] == FINISHED)
				{
					continue;
				}

				int emitterSize = emitterData->myEmitters[k].Size();
				for (int j = 0; j < emitterSize; ++j)
				{
					Prism::ParticleEmitterInstance* instance = emitterData->myEmitters[k][j];
					if (instance->IsActive() == false)
					{
						continue;
					}

					instance->Update(aDeltaTime);
				}
			}
		}
	}

	void EmitterManager::RenderEmitters(Prism::Texture* aTexture)
	{
		Prism::ParticleDataContainer::GetInstance()->SetGPUData(*myCamera, aTexture);
		for (int i = 0; i < myEmitterList.Size(); ++i)
		{
			EmitterData* emitter = myEmitterList[i];
			if (emitter->myTypeIsActive == false)
			{
				continue;
			}
			for (int k = 0; k < PREALLOCATED_EMITTERGROUP; k++)
			{
				int finished = 0;
				if (emitter->myFinishedGroups[k] == FINISHED)
				{
					continue;
				}

				for (int j = 0; j < emitter->myEmitters[k].Size(); ++j)
				{
					Prism::ParticleEmitterInstance* instance = emitter->myEmitters[k][j];
					if (instance->IsActive() == false)
					{
						finished++;

					}
					else
					{
						CU::Vector3<float> pos = instance->GetPosition() - myCamera->GetOrientation().GetPos();
						instance->Render();
					}
				}

				if (finished >= emitter->myEmitters[k].Size())
				{
					emitter->myActiveCount--;
					emitter->myFinishedGroups[k] = FINISHED;
					if (emitter->myActiveCount <= 0)
					{
						emitter->myTypeIsActive = false;
					}
				}
			}
		}
	}

	void EmitterManager::ReceiveMessage(const EmitterMessage& aMessage)
	{

		CU::Vector3f position = aMessage.myPosition;

		if (aMessage.myShouldKillEmitter == true)
		{
			if (aMessage.myEmitter != nullptr)
			{
				aMessage.myEmitter->KillEmitter(1.f);
			}
			return;
		}

		std::string particleType = CU::ToLower(aMessage.myParticleTypeString);
		//if (myEmitters.find(particleType) == myEmitters.end())
		//{
		//	return;
		//}
		DL_ASSERT_EXP(myEmitters.find(particleType) != myEmitters.end(), "Effect did not exist!");

		EmitterData* emitter = myEmitters[particleType];

		if (emitter->myCurrentIndex > (PREALLOCATED_EMITTERGROUP - 1))
		{
			emitter->myCurrentIndex = 0;
		}


		short index = emitter->myCurrentIndex;

		for (int i = 0; i < emitter->myEmitters[index].Size(); ++i)
		{
			Prism::ParticleEmitterInstance* instance = emitter->myEmitters[index][i];
			if (instance->IsActive() == true)
				continue;

			instance->SetEntity(aMessage.myEntity);

			if (aMessage.myEntity != nullptr)
			{
				aMessage.myEntity->AddEmitter(instance);
			}

			instance->Activate();

			instance->SetPosition(position);

			if (aMessage.myEmitterLifeTime > 0.f)
			{
				instance->SetEmitterLifeTime(aMessage.myEmitterLifeTime);
			}
			if (aMessage.myRadius > 0.f)
			{
				instance->SetRadius(aMessage.myRadius);
			}
			if ((aMessage.mySize.x > 0.f) && (aMessage.mySize.y > 0.f) && (aMessage.mySize.z > 0.f))
			{
				instance->SetSize(aMessage.mySize);
			}

			if (aMessage.myShouldKillEmitter == true)
			{
				instance->SetEmitterLifeTime(aMessage.myEmitterLifeTime);
			}

			if (aMessage.myUseDirection == true)
			{
				instance->SetDirection(aMessage.myDirection);
			}

			if (aMessage.myRotation.x > 0.f || aMessage.myRotation.y > 0.f || aMessage.myRotation.z > 0.f)
			{
				instance->SetRotation(aMessage.myRotation);
			}

			if (aMessage.myRandomDirection == true)
			{
				instance->SetRandomizeDirection(aMessage.myRandomDirection);
			}

			if (aMessage.myIsRotated == true)
			{
				instance->CalcRotation(aMessage.myDirection);
			}

		}

		emitter->myFinishedGroups[index] = UNFINISHED;
		emitter->myActiveCount++;
		emitter->myTypeIsActive = true;
		emitter->myCurrentIndex++;
	}

	void EmitterManager::ReadListOfLists(const std::string& aPath)
	{
		XMLReader rootDocument;
		rootDocument.OpenDocument(aPath);
		tinyxml2::XMLElement* rootElement = rootDocument.FindFirstChild("root");
		for (tinyxml2::XMLElement* e = rootDocument.FindFirstChild(rootElement); e != nullptr; e = rootDocument.FindNextElement(e))
		{
			std::string entityPath = "";
			rootDocument.ForceReadAttribute(e, "src", entityPath);
			std::string ID;
			rootDocument.ForceReadAttribute(e, "ID", ID);
			ID = CU::ToLower(ID);
			if (entityPath != "")
			{
				EmitterData* newData = new EmitterData(entityPath);
				myEmitters[ID] = newData;

				for (short i = 0; i < PREALLOCATED_EMITTERGROUP; ++i)
				{
					ReadList(entityPath, ID, i);

				}
			}
		}
		rootDocument.CloseDocument();
	}

	void EmitterManager::ReadList(const std::string& aPath, const std::string& anID, short anIndex)
	{
		XMLReader rootDocument;
		rootDocument.OpenDocument(aPath);
		tinyxml2::XMLElement* rootElement = rootDocument.FindFirstChild("root");
		for (tinyxml2::XMLElement* e = rootDocument.FindFirstChild(rootElement); e != nullptr;
			e = rootDocument.FindNextElement(e))
		{
			std::string entityPath = "";
			rootDocument.ForceReadAttribute(e, "src", entityPath);
			Prism::ParticleEmitterInstance* newEmitter;
			newEmitter = new Prism::ParticleEmitterInstance();
			newEmitter->Initiate(Prism::ParticleDataContainer::GetInstance()->
				GetParticleData(entityPath), true);
			myEmitters[anID]->myEmitters[anIndex].Add(newEmitter);

		}
		rootDocument.CloseDocument();
	}


	//Data
	EmitterData::EmitterData(const std::string& aType)
		: myType(aType)
		, myCurrentIndex(0)
		, myActiveCount(0)
		, myTypeIsActive(false)
	{
		for (int i = 0; i < PREALLOCATED_EMITTERGROUP; ++i)
		{
			myEmitters[i].Init(16);
		}
		myFinishedGroups.reset();
	}

	EmitterData::~EmitterData()
	{
		for (unsigned short i = 0; i < PREALLOCATED_EMITTERGROUP; ++i)
		{
			myEmitters[i].DeleteAll();
		}
	}
};