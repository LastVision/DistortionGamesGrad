#include "stdafx.h"
#include "AcidComponent.h"
#include "EntityFactory.h"
#include <MathHelper.h>
#include "PhysicsComponent.h"

AcidComponent::AcidComponent(Entity& anEntity)
	: Component(anEntity)
	, myAcidTimer(0.f)
	, myAcidIndex(0)
	, myAcidIntervalMin(0.f)
	, myAcidIntervalMax(0.f)
{
	mySpawnPosition = myEntity.GetOrientation().GetPos() + myEntity.GetOrientation().GetUp() * 2.f;
}

AcidComponent::~AcidComponent()
{
	myAcidDrops.DeleteAll();
}

void AcidComponent::Update(float aDeltaTime)
{
	for each (Entity* acidDrop in myAcidDrops)
	{
		acidDrop->Update(aDeltaTime);

		if (acidDrop->ShouldBeRemoved() == true)
		{
			acidDrop->GetComponent<PhysicsComponent>()->TeleportToPosition({ 0.f, -10.f, 0.f });
			acidDrop->RemoveFromScene();
			acidDrop->SetShouldBeRemoved(false);
		}
	}

	myAcidTimer -= aDeltaTime;

	if (myAcidTimer <= 0.f)
	{
		if (myAcidDrops[myAcidIndex]->IsInScene() == false)
		{
			myAcidDrops[myAcidIndex]->AddToScene();
		}

		CU::Vector3<float> velocity = CU::Math::RandomVector(mySpawnVelocityMin, mySpawnVelocityMax);

		myAcidDrops[myAcidIndex]->SetShouldBeRemoved(false);
		myAcidDrops[myAcidIndex]->GetComponent<PhysicsComponent>()->TeleportToPosition(mySpawnPosition);
		myAcidDrops[myAcidIndex]->GetComponent<PhysicsComponent>()->SetVelocity(velocity);

		myAcidTimer = CU::Math::RandomRange(myAcidIntervalMin, myAcidIntervalMax);

		myAcidIndex++;

		if (myAcidIndex >= myAcidDrops.Size())
		{
			myAcidIndex = 0;
		}
	}
}

void AcidComponent::InitAcid(int anAmount, float aAcidIntervalMax, float aAcidIntervalMin
	, CU::Vector3<float> aMaxVelocity, CU::Vector3<float> aMinVelocity, Prism::Scene* aScene)
{
	myAcidDrops.Init(anAmount);

	for (int i = 0; i < anAmount; i++)
	{
		Entity* acidDrop = EntityFactory::GetInstance()->CreateEntity(eEntityType::ACID_DROP, aScene, mySpawnPosition);
		acidDrop->GetComponent<PhysicsComponent>()->AddToScene();
		acidDrop->GetComponent<PhysicsComponent>()->TeleportToPosition({ -1.f * i, -10.f, 0.f });

		myAcidDrops.Add(acidDrop);
	}

	myAcidIntervalMax = aAcidIntervalMax;
	myAcidIntervalMin = aAcidIntervalMin;

	mySpawnVelocityMax = aMaxVelocity;
	mySpawnVelocityMin = aMinVelocity;

}