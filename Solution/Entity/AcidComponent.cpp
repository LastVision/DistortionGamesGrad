#include "stdafx.h"
#include "AcidComponent.h"
#include "EntityFactory.h"
#include "PhysicsComponent.h"

AcidComponent::AcidComponent(Entity& anEntity, Prism::Scene* aScene)
	: Component(anEntity)
	, myAcidDrops(8)
	, myAcidTimer(0.f)
	, myAcidIndex(0)
	, myAcidInterval(5.f)
{
	for (int i = 0; i < 4; i++)
	{
		mySpawnPosition = myEntity.GetOrientation().GetPos() + myEntity.GetOrientation().GetUp() * 2.f;

		Entity* acidDrop = EntityFactory::GetInstance()->CreateEntity(eEntityType::ACID_DROP, aScene, mySpawnPosition);
		myAcidDrops.Add(acidDrop);
	}
}

AcidComponent::~AcidComponent()
{
	myAcidDrops.DeleteAll();
}

void AcidComponent::Update(float aDeltaTime)
{
	myAcidTimer -= aDeltaTime;

	if (myAcidTimer <= 0.f)
	{
		if (myAcidDrops[myAcidIndex]->IsInScene() == false)
		{
			myAcidDrops[myAcidIndex]->GetComponent<PhysicsComponent>()->AddToScene();
			myAcidDrops[myAcidIndex]->AddToScene();
		}

		myAcidDrops[myAcidIndex]->SetShouldBeRemoved(false);
		myAcidDrops[myAcidIndex]->GetComponent<PhysicsComponent>()->TeleportToPosition(mySpawnPosition);

		myAcidIndex++;

		myAcidTimer = myAcidInterval;

		if (myAcidIndex >= myAcidDrops.Size())
		{
			myAcidIndex = 0;
		}
	}

	for each (Entity* acidDrop in myAcidDrops)
	{
		acidDrop->Update(aDeltaTime);

		if (acidDrop->ShouldBeRemoved() == true)
		{
			acidDrop->GetComponent<PhysicsComponent>()->RemoveFromScene();
			acidDrop->RemoveFromScene();
			acidDrop->SetShouldBeRemoved(false);
		}
	}
}