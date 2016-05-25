#include "stdafx.h"

#include <Entity.h>
#include <EntityFactory.h>
#include <PhysicsComponent.h>
#include <PostMaster.h>
#include "ScrapManager.h"
#include <ScrapMessage.h>

ScrapManager::ScrapManager(Prism::Scene* aScene, int aPlayerID)
	: myPlayerID(aPlayerID)
	, myHeads(16)
	, myLiveHeads(16)
	, myHeadIndex(0)
	, myLegs(16)
	, myLiveLegs(16)
	, myLegIndex(0)
	, myBodies(16)
	, myLiveBodies(16)
	, myBodyIndex(0)
	, myGibs(16)
	, myLiveGibs(16)
	, myGibIndex(0)
	, myScene(aScene)
{
	PostMaster::GetInstance()->Subscribe(this, eMessageType::SPAWN_SCRAP);
	CreateHeads();
	CreateLegs();
	CreateBodies();
	CreateGibs();
}


ScrapManager::~ScrapManager()
{
	PostMaster::GetInstance()->UnSubscribe(this, 0);
	for (int i = 0; i < myHeads.Size(); ++i)
	{
		SAFE_DELETE(myHeads[i].myEntity);
	}
	for (int i = 0; i < myBodies.Size(); ++i)
	{
		SAFE_DELETE(myBodies[i].myEntity);
	}
	for (int i = 0; i < myLegs.Size(); ++i)
	{
		SAFE_DELETE(myLegs[i].myEntity);
	}
	for (int i = 0; i < myGibs.Size(); ++i)
	{
		SAFE_DELETE(myGibs[i].myScrew);
		SAFE_DELETE(myGibs[i].myScrewNut);
		SAFE_DELETE(myGibs[i].mySpring);
	}
}

void ScrapManager::Update(float aDeltaTime)
{
	for (int i = myLiveHeads.Size() - 1; i >= 0; --i)
	{
		myLiveHeads[i]->myTimer += aDeltaTime;
		myLiveHeads[i]->myEntity->Update(aDeltaTime);

		if (myLiveHeads[i]->myTimer >= myLiveHeads[i]->myMaxTime)
		{
			myLiveHeads[i]->myEntity->RemoveFromScene();
			//myLiveHeads[i]->myEntity->GetComponent<PhysicsComponent>()->RemoveFromScene();
			myLiveHeads.RemoveCyclicAtIndex(i);
		}
	}
	DEBUG_PRINT(myLiveHeads.Size());

	for (int i = myLiveBodies.Size() - 1; i >= 0; --i)
	{
		myLiveBodies[i]->myTimer += aDeltaTime;
		myLiveBodies[i]->myEntity->Update(aDeltaTime);

		if (myLiveBodies[i]->myTimer >= myLiveBodies[i]->myMaxTime)
		{
			myLiveBodies[i]->myEntity->RemoveFromScene();
			//myLiveBodies[i]->myEntity->GetComponent<PhysicsComponent>()->RemoveFromScene();
			myLiveBodies.RemoveCyclicAtIndex(i);
		}
	}

	for (int i = myLiveLegs.Size() - 1; i >= 0; --i)
	{
		myLiveLegs[i]->myTimer += aDeltaTime;
		myLiveLegs[i]->myEntity->Update(aDeltaTime);

		if (myLiveLegs[i]->myTimer >= myLiveLegs[i]->myMaxTime)
		{
			myLiveLegs[i]->myEntity->RemoveFromScene();
			//myLiveLegs[i]->myEntity->GetComponent<PhysicsComponent>()->RemoveFromScene();
			myLiveLegs.RemoveCyclicAtIndex(i);
		}
	}

	for (int i = myLiveGibs.Size() - 1; i >= 0; --i)
	{
		myLiveGibs[i]->myTimer += aDeltaTime;
		myLiveGibs[i]->myScrew->Update(aDeltaTime);
		myLiveGibs[i]->myScrewNut->Update(aDeltaTime);
		myLiveGibs[i]->mySpring->Update(aDeltaTime);

		if (myLiveGibs[i]->myTimer >= myLiveGibs[i]->myMaxTime)
		{
			myLiveGibs[i]->myScrew->RemoveFromScene();
			myLiveGibs[i]->myScrewNut->RemoveFromScene();
			myLiveGibs[i]->mySpring->RemoveFromScene();
			//myLiveGibs[i]->myScrew->GetComponent<PhysicsComponent>()->RemoveFromScene();
			//myLiveGibs[i]->myScrewNut->GetComponent<PhysicsComponent>()->RemoveFromScene();
			//myLiveGibs[i]->mySpring->GetComponent<PhysicsComponent>()->RemoveFromScene();
			myLiveGibs.RemoveCyclicAtIndex(i);
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

		BodyPart* toAdd = &myHeads[myHeadIndex];
		toAdd->myTimer = 0.f;
		bool isAlreadyInScene = toAdd->myEntity->IsInScene();
		if (isAlreadyInScene == false)
		{
			DL_ASSERT_EXP(myLiveHeads.Find(toAdd) == myLiveHeads.FoundNone, "ERROR");
			myLiveHeads.Add(toAdd);

			toAdd->myEntity->AddToScene();
			if (toAdd->myEntity->GetComponent<PhysicsComponent>()->IsInScene() == false)
			{
				toAdd->myEntity->GetComponent<PhysicsComponent>()->AddToScene();
			}
		}

		toAdd->myEntity->GetComponent<PhysicsComponent>()->TeleportToPosition(aPosition);
		CU::Vector3<float> dir(aVelocity.x, aVelocity.y, 0.f);
		CU::Normalize(dir);
		toAdd->myEntity->GetComponent<PhysicsComponent>()->AddForce(dir, 10.f);


		++myHeadIndex;
		break;
	}
	case eScrapPart::BODY:
	{
		if (myBodyIndex >= myBodies.Size())
		{
			myBodyIndex = 0;
		}

		BodyPart* toAdd = &myBodies[myBodyIndex];
		toAdd->myTimer = 0.f;
		bool isAlreadyInScene = toAdd->myEntity->IsInScene();
		if (isAlreadyInScene == false)
		{
			myLiveBodies.Add(toAdd);

			toAdd->myEntity->AddToScene();
			if (toAdd->myEntity->GetComponent<PhysicsComponent>()->IsInScene() == false)
			{
				toAdd->myEntity->GetComponent<PhysicsComponent>()->AddToScene();
			}
		}
		toAdd->myEntity->GetComponent<PhysicsComponent>()->TeleportToPosition(aPosition);
		CU::Vector3<float> dir(aVelocity.x, aVelocity.y, 0.f);
		CU::Normalize(dir);
		toAdd->myEntity->GetComponent<PhysicsComponent>()->AddForce(dir, 10.f);


		++myBodyIndex;
		break;
	}
	case eScrapPart::LEGS:
	{
		if (myLegIndex >= myLegs.Size())
		{
			myLegIndex = 0;
		}

		BodyPart* toAdd = &myLegs[myLegIndex];
		toAdd->myTimer = 0.f;
		bool isAlreadyInScene = toAdd->myEntity->IsInScene();
		if (isAlreadyInScene == false)
		{
			myLiveLegs.Add(toAdd);

			toAdd->myEntity->AddToScene();
			if (toAdd->myEntity->GetComponent<PhysicsComponent>()->IsInScene() == false)
			{
				toAdd->myEntity->GetComponent<PhysicsComponent>()->AddToScene();
			}
		}
		toAdd->myEntity->GetComponent<PhysicsComponent>()->TeleportToPosition(aPosition);
		CU::Vector3<float> dir(aVelocity.x, aVelocity.y, 0.f);
		CU::Normalize(dir);
		toAdd->myEntity->GetComponent<PhysicsComponent>()->AddForce(dir, 10.f);


		++myLegIndex;
		break;
	}
	case eScrapPart::GIBS:
	{
		if (myGibIndex >= myGibs.Size())
		{
			myGibIndex = 0;
		}

		GibPart* toAdd = &myGibs[myGibIndex];
		toAdd->myTimer = 0.f;
		bool isAlreadyInScene = toAdd->myScrew->IsInScene();
		if (isAlreadyInScene == false)
		{
			myLiveGibs.Add(toAdd);

			toAdd->myScrew->AddToScene();
			toAdd->myScrewNut->AddToScene();
			toAdd->mySpring->AddToScene();
			if (toAdd->myScrew->GetComponent<PhysicsComponent>()->IsInScene() == false)
			{
				toAdd->myScrew->GetComponent<PhysicsComponent>()->AddToScene();
			}
			if (toAdd->myScrewNut->GetComponent<PhysicsComponent>()->IsInScene() == false)
			{
				toAdd->myScrewNut->GetComponent<PhysicsComponent>()->AddToScene();
			}
			if (toAdd->mySpring->GetComponent<PhysicsComponent>()->IsInScene() == false)
			{
				toAdd->mySpring->GetComponent<PhysicsComponent>()->AddToScene();
			}
		}
		CU::Vector3<float> offset;

		toAdd->myScrew->GetComponent<PhysicsComponent>()->TeleportToPosition(aPosition + offset);
		offset.y = 0.5f;
		toAdd->myScrewNut->GetComponent<PhysicsComponent>()->TeleportToPosition(aPosition + offset);
		offset.y = -0.5f;
		toAdd->mySpring->GetComponent<PhysicsComponent>()->TeleportToPosition(aPosition + offset);
		CU::Vector3<float> dir(aVelocity.x, aVelocity.y, 0.f);
		CU::Normalize(dir);
		dir.z = (rand() % 100) * 0.01f;
		toAdd->myScrew->GetComponent<PhysicsComponent>()->AddForce(dir, 0.f);
		toAdd->myScrewNut->GetComponent<PhysicsComponent>()->AddForce(dir, 0.f);
		toAdd->mySpring->GetComponent<PhysicsComponent>()->AddForce(dir, 0.f);



		++myGibIndex;
		break;
	}

	default:
		break;
	}
}

void ScrapManager::ReceiveMessage(const ScrapMessage& aMessage)
{
	if (myPlayerID == aMessage.myPlayerID)
	{
		SpawnScrap(aMessage.myScrapPart, aMessage.myPosition, aMessage.myVelocity);
		SpawnScrap(eScrapPart::GIBS, aMessage.myPosition, aMessage.myVelocity);
	}

}

void ScrapManager::CreateHeads()
{
	std::string headName("head");
	headName += std::to_string(myPlayerID);
	for (int i = 0; i < myHeads.GetCapacity(); ++i)
	{
		BodyPart toAdd;
		toAdd.myEntity = EntityFactory::CreateEntity(eEntityType::SCRAP, headName, myScene, { 1000.f, 10000.f + (i * 100.f), 10000.f });
		myHeads.Add(toAdd);
	}
}

void ScrapManager::CreateLegs()
{
	std::string legsName("legs");
	legsName += std::to_string(myPlayerID);
	for (int i = 0; i < myLegs.GetCapacity(); ++i)
	{
		BodyPart toAdd;
		toAdd.myEntity = EntityFactory::CreateEntity(eEntityType::SCRAP, legsName, myScene, { 1000.f, 100000.f + (i * 100.f), 10000.f });
		myLegs.Add(toAdd);
	}
}

void ScrapManager::CreateBodies()
{
	std::string bodyName("body");
	bodyName += std::to_string(myPlayerID);
	for (int i = 0; i < myBodies.GetCapacity(); ++i)
	{
		BodyPart toAdd;
		toAdd.myEntity = EntityFactory::CreateEntity(eEntityType::SCRAP, bodyName, myScene, { 1000.f, 150000.f + (i * 100.f), 10000.f });
		myBodies.Add(toAdd);
	}
}

void ScrapManager::CreateGibs()
{
	for (int i = 0; i < myGibs.GetCapacity(); ++i)
	{
		GibPart toAdd;
		toAdd.mySpring = EntityFactory::CreateEntity(eEntityType::SCRAP, "gibspring", myScene, { 1000.f, 150000.f + (i * 100.f), 10000.f });
		toAdd.myScrewNut = EntityFactory::CreateEntity(eEntityType::SCRAP, "gibscrewnut", myScene, { 1000.f, 150000.f + (i * 100.f), 10000.f });
		toAdd.myScrew = EntityFactory::CreateEntity(eEntityType::SCRAP, "gibscrew", myScene, { 1000.f, 150000.f + (i * 100.f), 10000.f });
		myGibs.Add(toAdd);
	}
}