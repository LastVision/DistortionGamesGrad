#include "stdafx.h"
#include "AudioInterface.h"
#include <EmitterMessage.h>
#include "EntityFactory.h"
#include "SteamComponent.h"
#include "TriggerComponent.h"
#include "PhysicsComponent.h"
#include <PostMaster.h>
#include "SoundComponent.h"
#include "SteamVentNote.h"
#include <SoundMessage.h>

SteamComponent::SteamComponent(Entity& anEntity)
	: Component(anEntity)
	, myCurrentSteamInterval(0.f)
	, myCurrentSteamTime(0.f)
	, myDelayBeforeSteam(0.f)
	, mySteamInterval(0.f)
	, mySteamTime(0.f)
	, myIsConstant(true)
	, mySteam(nullptr)
	, myTimeBeforeVentOpens(0.5f)
	, myTimeBeforeSmoke(0.3f)
	, myIsSmoking(false)
	, myIsOpen(false)
{
	PostMaster::GetInstance()->Subscribe(this, eMessageType::SOUND);
}

SteamComponent::~SteamComponent()
{
	SAFE_DELETE(mySteam);
	SoundComponent* soundComp = myEntity.GetComponent<SoundComponent>();
	if (soundComp != nullptr)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_Steam", soundComp->GetAudioSFXID());
	}
	PostMaster::GetInstance()->UnSubscribe(this, eMessageType::SOUND);
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
				mySteam->GetComponent<PhysicsComponent>()->RemoveFromScene();
				myIsSmoking = false;
				myIsOpen = false;
			}
			else if (myIsOpen == false && myCurrentSteamTime <= myTimeBeforeVentOpens)
			{
				myIsOpen = true;
				myEntity.SendNote(SteamVentNote(false));
			}
			else if (myIsSmoking == false && myCurrentSteamTime <= myTimeBeforeSmoke)
			{
				myIsSmoking = true;
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
				mySteam->GetComponent<PhysicsComponent>()->AddToScene();
				myIsSmoking = false;
				myIsOpen = false;
			}
			else if (myIsOpen == false && myCurrentSteamInterval <= myTimeBeforeVentOpens)
			{
				myIsOpen = true;
				myEntity.SendNote(SteamVentNote(true));
				PostMaster::GetInstance()->SendMessage(EmitterMessage("Steam_Heat", myEntity.GetOrientation().GetPos(), myEntity.GetOrientation().GetUp(), mySteamTime));

			}
			else if (myIsSmoking == false && myCurrentSteamInterval <= myTimeBeforeSmoke)
			{
				myIsSmoking = true;
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

void SteamComponent::InitSteam(Prism::Scene* aScene, const CU::Vector3<float>& aRotation, const std::string& aSteamSubType, float aSteamInterval, float aSteamTime, float aDelayBeforeSteam)
{
	mySteam = EntityFactory::GetInstance()->CreateEntity(eEntityType::STEAM, aSteamSubType, aScene, myEntity.GetOrientation().GetPos(), { 0.f, aRotation.y, aRotation.z });

	mySteam->GetComponent<PhysicsComponent>()->AddToScene();
	mySteam->GetComponent<PhysicsComponent>()->UpdateOrientationStatic();

	mySteamInterval = aSteamInterval;
	mySteamTime = aSteamTime;
	myDelayBeforeSteam = aDelayBeforeSteam;
	myIsConstant = false;

	if (mySteamInterval == 0.f && mySteamTime == 0.f)
	{
		myIsConstant = true;
		PostMaster::GetInstance()->SendMessage(EmitterMessage("Steam_Constant", myEntity.GetOrientation().GetPos(), myEntity.GetOrientation().GetUp()));
		SoundComponent* soundComp = myEntity.GetComponent<SoundComponent>();
		myEntity.SendNote(SteamVentNote(true));
		if (soundComp != nullptr)
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Steam", soundComp->GetAudioSFXID());
		}
	}

	myCurrentSteamTime = mySteamTime;
	myCurrentSteamInterval = 0.f;

	if (myDelayBeforeSteam > 0.f)
	{
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

void SteamComponent::ReceiveMessage(const SoundMessage& aMessage)
{
	SoundComponent* soundComp = myEntity.GetComponent<SoundComponent>();
	if (aMessage.myShouldPlaySound == true)
	{
		if (soundComp != nullptr)
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Steam", soundComp->GetAudioSFXID());
		}
	}
	else
	{
		if (soundComp != nullptr)
		{
			Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_Steam", soundComp->GetAudioSFXID());
		}
	}
}