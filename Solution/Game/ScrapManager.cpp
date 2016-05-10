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
	, myScene(aScene)
{
	PostMaster::GetInstance()->Subscribe(this, eMessageType::SPAWN_SCRAP);
	CreateHeads();
}


ScrapManager::~ScrapManager()
{
	PostMaster::GetInstance()->UnSubscribe(this, 0);
	for (int i = 0; i < myHeads.Size(); ++i)
	{
		SAFE_DELETE(myHeads[i].myHead);
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
		myLiveHeads[i].myHead->Update(aDeltaTime);

		if (myLiveHeads[i].myTimer >= myLiveHeads[i].myMaxTime)
		{
			myLiveHeads[i].myHead->RemoveFromScene();
			myLiveHeads[i].myHead->GetComponent<PhysicsComponent>()->RemoveFromScene();
			myLiveHeads.RemoveCyclicAtIndex(i);
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

		Head toAdd; 
		toAdd.myHead = myHeads[myHeadIndex].myHead;
		toAdd.myMaxTime = myHeads[myHeadIndex].myMaxTime;
		myLiveHeads.Add(toAdd);

		myLiveHeads.GetLast().myHead->AddToScene();
		myLiveHeads.GetLast().myHead->GetComponent<PhysicsComponent>()->AddToScene();
		myLiveHeads.GetLast().myHead->GetComponent<PhysicsComponent>()->TeleportToPosition(aPosition);
		CU::Vector3<float> dir(aVelocity.x, aVelocity.y, 0.f);
		CU::Normalize(dir);
		myLiveHeads.GetLast().myHead->GetComponent<PhysicsComponent>()->AddForce(dir, 50.f);
		++myHeadIndex;
		break;
	}
	case eScrapPart::BODY:
		DL_ASSERT("BODY not implemented!");
		break;
	case eScrapPart::LEGS:
		DL_ASSERT("LEGS not implemented!");
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
		Head toAdd;
		toAdd.myHead = EntityFactory::CreateEntity(eEntityType::SCRAP, "head", myScene, { 1000.f, 10000.f + (i * 100.f), 10000.f });
		myHeads.Add(toAdd);
	}
}
