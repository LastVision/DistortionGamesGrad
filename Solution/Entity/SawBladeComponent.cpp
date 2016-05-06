#include "stdafx.h"
#include "SawBladeComponent.h"
#include "PhysicsComponent.h"

SawBladeComponent::SawBladeComponent(Entity& anEntity)
	: Component(anEntity)
	, myPatrolSpeed(0.f)
	, myDelayBeforePatrol(0.f)
	, myIsLoopingForward(true)
	, myPositions(8)
	, myCurrentIndex(0)
{
}

SawBladeComponent::~SawBladeComponent()
{
	myPositions.RemoveAll();
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
		CU::Vector3<float> direction = (myPositions[myCurrentIndex] - position);
		CU::Normalize(direction);

		position += direction * aDeltaTime * myPatrolSpeed;

		myEntity.SetPosition(position);

		if (position.x <= myPositions[myCurrentIndex].x + 0.1f &&
			position.x >= myPositions[myCurrentIndex].x - 0.1f &&
			position.y <= myPositions[myCurrentIndex].y + 0.1f &&
			position.y >= myPositions[myCurrentIndex].y - 0.1f)
		{
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
		}
	}
}