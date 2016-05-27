#include "stdafx.h"

#include <AudioInterface.h>
#include "SawBladeComponent.h"
#include "SoundComponent.h"
#include "PhysicsComponent.h"

SawBladeComponent::SawBladeComponent(Entity& anEntity)
	: Component(anEntity)
	, myPatrolSpeed(0.f)
	, myDelayBeforePatrol(0.f)
	, myIsLoopingForward(true)
	, myPositions(8)
	, myCurrentIndex(0)
	, myTotalCurrentLength(0.f)
{
}

SawBladeComponent::~SawBladeComponent()
{
	myPositions.RemoveAll();
	SoundComponent* soundComp = myEntity.GetComponent<SoundComponent>();
	if (soundComp != nullptr)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_Saw", soundComp->GetAudioSFXID());
	}
}

void SawBladeComponent::Init()
{
	SoundComponent* soundComp = myEntity.GetComponent<SoundComponent>();
	if (soundComp != nullptr)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Saw", soundComp->GetAudioSFXID());
	}
}

void SawBladeComponent::Update(float aDeltaTime)
{
	if (myDelayBeforePatrol > 0.f)
	{
		myDelayBeforePatrol -= aDeltaTime;
		return;
	}

	if (myPositions.Size() > 0)
	{
		CU::Vector3<float> position = myEntity.GetOrientation().GetPos();
		CU::Vector3<float> direction = myPositions[myCurrentIndex] - position;
		CU::Normalize(direction);

		position += direction * aDeltaTime * myPatrolSpeed;
		myEntity.SetPosition(position);

		float length = CU::Length(position - myStartPosition);

		if (length >= myTotalCurrentLength)
		{
			float remainder = length - myTotalCurrentLength;
			position = myPositions[myCurrentIndex];

			if (myIsLoopingForward == true)
			{
				myCurrentIndex++;
				if (myCurrentIndex + 1 == myPositions.Size())
				{
					myIsLoopingForward = false;
				}
			}
			else
			{
				myCurrentIndex--;
				if (myCurrentIndex == 0)
				{
					myIsLoopingForward = true;
				}
			}

			if (remainder > 0.f)
			{
				myEntity.SetPosition(position);
				position = myEntity.GetOrientation().GetPos();
				direction = myPositions[myCurrentIndex] - position;
				CU::Normalize(direction);
				position += direction * remainder;
				myEntity.SetPosition(position);
			}	

			myStartPosition = myEntity.GetOrientation().GetPos();
			myTotalCurrentLength = CU::Length(myStartPosition - myPositions[myCurrentIndex]);
		}
	}
}

void SawBladeComponent::SetPatrol(const CU::GrowingArray<CU::Vector3<float>>& somePositions, float aSpeed, float aDelayBeforePatrol)
{
	myPositions = somePositions;
	myPatrolSpeed = aSpeed;
	myDelayBeforePatrol = aDelayBeforePatrol;

	if (myPositions.Size() > 0)
	{
		myStartPosition = myEntity.GetOrientation().GetPos();
		myTotalCurrentLength = CU::Length(myStartPosition - myPositions[0]);
	}
}