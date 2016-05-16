#include "stdafx.h"
#include "AudioInterface.h"
#include <EmitterMessage.h>
#include "EntityFactory.h"
#include "SteamComponent.h"
#include "TriggerComponent.h"
#include "PhysicsComponent.h"
#include <PostMaster.h>
#include "SoundComponent.h"
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


	//mySteam->AddToScene();
	mySteam->GetComponent<PhysicsComponent>()->AddToScene();
	//mySteam->GetComponent<PhysicsComponent>()->TeleportToPosition(mySteam->GetOrientation().GetPos() + (myEntity.GetOrientation().GetUp()));
	mySteam->GetComponent<PhysicsComponent>()->UpdateOrientationStatic();
}

SteamComponent::~SteamComponent()
{
	SAFE_DELETE(mySteam);
	SoundComponent* soundComp = myEntity.GetComponent<SoundComponent>();
	if (soundComp != nullptr)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_Steam", soundComp->GetAudioSFXID());
	}
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
		SoundComponent* soundComp = myEntity.GetComponent<SoundComponent>();
		if (myCurrentSteamTime > 0.f)
		{
			myCurrentSteamTime -= aDeltaTime;
			if (myCurrentSteamTime <= 0.f)
			{
				myCurrentSteamInterval = mySteamInterval;
				myCurrentSteamTime = 0.f;
				//mySteam->RemoveFromScene();
				mySteam->GetComponent<PhysicsComponent>()->RemoveFromScene();
				if (soundComp != nullptr)
				{
					Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_Steam", soundComp->GetAudioSFXID());
				}
			}
		}
		else
		{
			myCurrentSteamInterval -= aDeltaTime;
			if (myCurrentSteamInterval <= 0.f)
			{
				myCurrentSteamInterval = 0.f;
				myCurrentSteamTime = mySteamTime;
				//mySteam->AddToScene();
				mySteam->GetComponent<PhysicsComponent>()->AddToScene();
				PostMaster::GetInstance()->SendMessage(EmitterMessage("Steam", myEntity.GetOrientation().GetPos(), myEntity.GetOrientation().GetUp(), mySteamTime));
				if (soundComp != nullptr)
				{
					Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Steam", soundComp->GetAudioSFXID());
				}

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
		PostMaster::GetInstance()->SendMessage(EmitterMessage("Steam", myEntity.GetOrientation().GetPos(), myEntity.GetOrientation().GetUp()));
		SoundComponent* soundComp = myEntity.GetComponent<SoundComponent>();
		if (soundComp != nullptr)
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Steam", soundComp->GetAudioSFXID());
		}
	}

	myCurrentSteamTime = mySteamTime;
	myCurrentSteamInterval = 0.f;

	if (myDelayBeforeSteam > 0.f)
	{
		//mySteam->RemoveFromScene();
		myCurrentSteamTime = 0.f;
		mySteam->GetComponent<PhysicsComponent>()->RemoveFromScene();
	}
}

float SteamComponent::GetForce() const
{
	return mySteam->GetComponent<TriggerComponent>()->GetForce();
}

void SteamComponent::SetForce(float aForce)
{
	mySteam->GetComponent<TriggerComponent>()->SetForce(aForce);
}