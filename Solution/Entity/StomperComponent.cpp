#include "stdafx.h"
#include "EntityFactory.h"
#include "PhysicsComponent.h"
#include "StomperComponent.h"

StomperComponent::StomperComponent(Entity& anEntity, Prism::Scene* aScene, const CU::Vector3<float>& aRotation)
	: Component(anEntity)
	, myBeforeStompTimer(0.f)
	, myStompDownTimer(0.f)
	, myTimeStomperDown(0.f)
	, myTimeBeforeStomp(0.f)
	, myStompSpeedOut(0.f)
	, myStompSpeedIn(0.f)
	, myDistance(0.f)
	, myToStompLength(0.f)
	, myHasReachedMax(false)
	, myHasReachedMin(true)
	, myDelayBeforeStomp(0.f)
{
	myStomper = EntityFactory::GetInstance()->CreateEntity(eEntityType::STOMPER, "stomper_small", aScene, myEntity.GetOrientation().GetPos(), aRotation);
	myStomper->AddToScene();
	myStomper->GetComponent<PhysicsComponent>()->AddToScene();
}

StomperComponent::~StomperComponent()
{
	SAFE_DELETE(myStomper);
}

void StomperComponent::Update(float aDeltaTime)
{
	if (myDelayBeforeStomp > 0.f)
	{
		myDelayBeforeStomp -= aDeltaTime;
	}
	else
	{
		CU::Vector3<float> currentStompPosition = myStomper->GetOrientation().GetPos();
		CU::Vector3<float> entityPosition = myEntity.GetOrientation().GetPos();

		if (myStompDownTimer > 0.f)
		{
			if (myHasReachedMax == false)
			{
				myStomper->SetStomperMoving(true);
				myStomper->SetRotation({ 0, 5.f * aDeltaTime, 0 });
				float length = CU::Length(entityPosition - currentStompPosition);

				if (length < myToStompLength)
				{
					CU::Vector3<float> direction = (myStomperMaxPosition - currentStompPosition);
					CU::Normalize(direction);

					CU::Vector3<float> nextPos = currentStompPosition + (direction * aDeltaTime * myStompSpeedOut);

					float nextLength = CU::Length(entityPosition - nextPos);
					if (nextLength < myToStompLength)
					{
						myStomper->SetPosition(nextPos);
					}
					else
					{
						myStomper->SetPosition(myStomperMaxPosition);
						myHasReachedMax = true;
						myStomper->SetStomperMoving(false);
					}
				}
			}
			else
			{
				myStomper->SetStomperMoving(false);
			}

			myStompDownTimer -= aDeltaTime;

			if (myStompDownTimer <= 0.f)
			{
				myStompDownTimer = 0.f;
				myBeforeStompTimer = myTimeBeforeStomp;
				myHasReachedMin = false;
			}
		}
		else
		{
			if (myHasReachedMin == false)
			{
				myStomper->SetStomperMoving(true);
				myStomper->SetRotation({ 0, -5.f * aDeltaTime, 0 });
				float length = CU::Length(currentStompPosition - myStomperMaxPosition);

				if (length < myToStompLength)
				{
					CU::Vector3<float> direction = (entityPosition - currentStompPosition);
					CU::Vector3<float> nextPos;

					CU::Normalize(direction);
					nextPos = currentStompPosition + (direction * aDeltaTime * myStompSpeedIn);

					float nextLength = CU::Length(myStomperMaxPosition - nextPos);

					if (nextLength < myToStompLength)
					{
						myStomper->SetPosition(nextPos);
					}
					else
					{
						myStomper->SetPosition(entityPosition);
						myHasReachedMin = true;
						myStomper->SetStomperMoving(false);
					}
				}
			}
			else
			{
				myStomper->SetStomperMoving(false);
			}

			myBeforeStompTimer -= aDeltaTime;

			if (myBeforeStompTimer <= 0.f)
			{
				myBeforeStompTimer = 0.f;
				myStompDownTimer = myTimeStomperDown;
				myHasReachedMax = false;
			}
		}
	}

	myStomper->Update(aDeltaTime);
}

void StomperComponent::InitStomper(float aTimeBeforeStomp, float aTimeStomperIsDown, float aStomperSpeedOut
	, float aStomperSpeedIn, float aStomperDistance, float aDelayBeforeStart)
{
	myTimeBeforeStomp = aTimeBeforeStomp;
	myTimeStomperDown = aTimeStomperIsDown;
	myStompSpeedOut = aStomperSpeedOut;
	myStompSpeedIn = aStomperSpeedIn;
	myDistance = aStomperDistance;
	myDelayBeforeStomp = aDelayBeforeStart;

	myStomperMaxPosition = myEntity.GetOrientation().GetPos() + (myEntity.GetOrientation().GetUp() * myStomper->GetComponent<PhysicsComponent>()->GetHeight() * myDistance);
	myToStompLength = CU::Length(myEntity.GetOrientation().GetPos() - myStomperMaxPosition);
}