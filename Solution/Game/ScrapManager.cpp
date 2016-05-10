#include "stdafx.h"

#include <Entity.h>
#include <EntityFactory.h>
#include <PhysicsComponent.h>
#include <PostMaster.h>
#include "ScrapManager.h"
#include <ScrapMessage.h>

ScrapManager* ScrapManager::myInstance = nullptr;

ScrapManager::ScrapManager(Prism::Scene* aScene)
	: myHeads(16)
	, myLiveHeads(16)
	, myHeadIndex(0)
	, myLegs(16)
	, myLiveLegs(16)
	, myLegIndex(0)
	, myScene(aScene)
{
	PostMaster::GetInstance()->Subscribe(this, eMessageType::SPAWN_SCRAP);
	CreateHeads();
	CreateLegs();
}


ScrapManager::~ScrapManager()
{
	PostMaster::GetInstance()->UnSubscribe(this, 0);
	for (int i = 0; i < myHeads.Size(); ++i)
	{
		SAFE_DELETE(myHeads[i].myEntity);
	}
}


void ScrapManager::Create(Prism::Scene* aScene)
{
	DL_ASSERT_EXP(myInstance == nullptr, "Tried to create ScrapManager while it already existed!");
	myInstance = new ScrapManager(aScene);
}

void ScrapManager::Destroy()
{
	DL_ASSERT_EXP(myInstance != nullptr, "Tried to create ScrapManager while it already existed!");
	SAFE_DELETE(myInstance);
}

ScrapManager* ScrapManager::GetInstance()
{
	DL_ASSERT_EXP(myInstance != nullptr, "ScrapManager were null!");
	return myInstance;
}

void ScrapManager::Update(float aDeltaTime)
{
	for (int i = myLiveHeads.Size() - 1; i >= 0; --i)
	{
		myLiveHeads[i].myTimer += aDeltaTime;
		myLiveHeads[i].myEntity->Update(aDeltaTime);

		if (myLiveHeads[i].myTimer >= myLiveHeads[i].myMaxTime)
		{
			myLiveHeads[i].myEntity->RemoveFromScene();
			myLiveHeads[i].myEntity->GetComponent<PhysicsComponent>()->RemoveFromScene();
			myLiveHeads.RemoveCyclicAtIndex(i);
		}
	}

	for (int i = myLiveLegs.Size() - 1; i >= 0; --i)
	{
		myLiveLegs[i].myTimer += aDeltaTime;
		myLiveLegs[i].myEntity->Update(aDeltaTime);

		if (myLiveLegs[i].myTimer >= myLiveLegs[i].myMaxTime)
		{
			myLiveLegs[i].myEntity->RemoveFromScene();
			myLiveLegs[i].myEntity->GetComponent<PhysicsComponent>()->RemoveFromScene();
			myLiveLegs.RemoveCyclicAtIndex(i);
		}
	}
}

void ScrapManager::SpawnScrap(eScrapPart aPart, const CU::Vector3<float>& aPosition, const CU::Vector2<float>& aVelocity)
{
	switch (aPart)
	{
	case eScrapPart::HEAD:
	{
		if (myHeadIndex >= myHeads.Size())
		{
			myHeadIndex = 0;
		}

		BodyPart toAdd; 
		toAdd.myEntity = myHeads[myHeadIndex].myEntity;
		toAdd.myMaxTime = myHeads[myHeadIndex].myMaxTime;
		myLiveHeads.Add(toAdd);

		myLiveHeads.GetLast().myEntity->AddToScene();
		myLiveHeads.GetLast().myEntity->GetComponent<PhysicsComponent>()->AddToScene();
		myLiveHeads.GetLast().myEntity->GetComponent<PhysicsComponent>()->TeleportToPosition(aPosition);
		CU::Vector3<float> dir(aVelocity.x, aVelocity.y, 0.f);
		CU::Normalize(dir);
		myLiveHeads.GetLast().myEntity->GetComponent<PhysicsComponent>()->AddForce(dir, 10.f);
		++myHeadIndex;
		break;
	}
	case eScrapPart::BODY:
		DL_ASSERT("BODY not implemented!");
		break;
	case eScrapPart::LEGS:
	{
		if (myLegIndex >= myLegs.Size())
		{
			myLegIndex = 0;
		}

		BodyPart toAdd;
		toAdd.myEntity = myLegs[myLegIndex].myEntity;
		toAdd.myMaxTime = myLegs[myLegIndex].myMaxTime;
		myLiveLegs.Add(toAdd);

		myLiveLegs.GetLast().myEntity->AddToScene();
		myLiveLegs.GetLast().myEntity->GetComponent<PhysicsComponent>()->AddToScene();
		myLiveLegs.GetLast().myEntity->GetComponent<PhysicsComponent>()->TeleportToPosition(aPosition);
		CU::Vector3<float> dir(aVelocity.x, aVelocity.y, 0.f);
		CU::Normalize(dir);
		myLiveLegs.GetLast().myEntity->GetComponent<PhysicsComponent>()->AddForce(dir, 10.f);
		++myLegIndex;
		break;
	}
		break;
	default:
		break;
	}
}

void ScrapManager::ReceiveMessage(const ScrapMessage& aMessage)
{
	SpawnScrap(aMessage.myScrapPart, aMessage.myPosition, aMessage.myVelocity);
}

void ScrapManager::CreateHeads()
{
	for (int i = 0; i < myHeads.GetCapacity(); ++i)
	{
		BodyPart toAdd;
		toAdd.myEntity = EntityFactory::CreateEntity(eEntityType::SCRAP, "head", myScene, { 1000.f, 10000.f + (i * 100.f), 10000.f });
		myHeads.Add(toAdd);
	}
}

void ScrapManager::CreateLegs()
{
	for (int i = 0; i < myLegs.GetCapacity(); ++i)
	{
		BodyPart toAdd;
		toAdd.myEntity = EntityFactory::CreateEntity(eEntityType::SCRAP, "legs", myScene, { 1000.f, 100000.f + (i * 100.f), 10000.f });
		myLegs.Add(toAdd);
	}
}