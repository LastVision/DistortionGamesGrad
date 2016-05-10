#include "stdafx.h"
#include "EntityFactory.h"
#include "SteamComponent.h"
#include "PhysicsComponent.h"

SteamComponent::SteamComponent(Entity& anEntity, Prism::Scene* aScene, const CU::Vector3<float>& aRotation)
	: Component(anEntity)
	, myCurrentSteamInterval(0.f)
	, myCurrentSteamTime(0.f)
	, myDelayBeforeSteam(0.f)
	, mySteamInterval(0.f)
	, mySteamTime(0.f)
	, myIsConstant(true)
{
	mySteam = EntityFactory::GetInstance()->CreateEntity(eEntityType::STEAM, aScene, myEntity.GetOrientation().GetPos(), aRotation);


	mySteam->AddToScene();
	mySteam->GetComponent<PhysicsComponent>()->AddToScene();
	//mySteam->GetComponent<PhysicsComponent>()->TeleportToPosition(mySteam->GetOrientation().GetPos() + (myEntity.GetOrientation().GetUp()));
	mySteam->GetComponent<PhysicsComponent>()->UpdateOrientationStatic();
}

SteamComponent::~SteamComponent()
{
	SAFE_DELETE(mySteam);
}

void SteamComponent::Update(float aDeltaTime)
{
	if (myIsConstant == false)
	{
		if (myDelayBeforeSteam > 0.f)
		{
			myDelayBeforeSteam -= aDeltaTime;
			return;
		}

		if (myCurrentSteamTime > 0.f)
		{
			myCurrentSteamTime -= aDeltaTime;
			if (myCurrentSteamTime <= 0.f)
			{
				myCurrentSteamInterval = mySteamInterval;
				myCurrentSteamTime = 0.f;
				mySteam->RemoveFromScene();
				mySteam->GetComponent<PhysicsComponent>()->RemoveFromScene();
			}
		}
		else
		{
			myCurrentSteamInterval -= aDeltaTime;
			if (myCurrentSteamInterval <= 0.f)
			{
				myCurrentSteamInterval = 0.f;
				myCurrentSteamTime = mySteamTime;
				mySteam->AddToScene();
				mySteam->GetComponent<PhysicsComponent>()->AddToScene();
			}
		}
	}

	mySteam->Update(aDeltaTime);
}

void SteamComponent::SetSteamVariables(float aSteamInterval, float aSteamTime, float aDelayBeforeSteam)
{
	mySteamInterval = aSteamInterval;
	mySteamTime = aSteamTime;
	myDelayBeforeSteam = aDelayBeforeSteam;
	myIsConstant = false;

	if (mySteamInterval == 0.f && mySteamTime == 0.f)
	{
		myIsConstant = true;
	}

	myCurrentSteamTime = mySteamTime;
	myCurrentSteamInterval = 0.f;

	if (myDelayBeforeSteam > 0.f)
	{
		mySteam->RemoveFromScene();
		myCurrentSteamTime = 0.f;
		mySteam->GetComponent<PhysicsComponent>()->RemoveFromScene();
	}
}