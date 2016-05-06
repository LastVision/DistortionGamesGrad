#include "stdafx.h"
#include "DashAimMovement.h"
#include "DashFlyMovement.h"
#include "MovementComponent.h"
#include "FlyMovement.h"
#include "WalkMovement.h"

MovementComponent::MovementComponent(Entity& aEntity, const MovementComponentData& aData, CU::Matrix44f& anOrientation, Prism::Scene* aScene)
	: Component(aEntity)
	, myData(aData)
	, myCurrentMovement(eMovementType::FLY)
	, myDashCooldown(0.f)
{
	myMovements[eMovementType::FLY] = new FlyMovement(aData, anOrientation, *this);
	myMovements[eMovementType::WALK] = new WalkMovement(aData, anOrientation, *this);
	myMovements[eMovementType::DASH_AIM] = new DashAimMovement(aData, anOrientation, *this, aScene);
	myMovements[eMovementType::DASH_FLY] = new DashFlyMovement(aData, anOrientation, *this);
}

MovementComponent::~MovementComponent()
{
	for (int i = 0; i < eMovementType::_COUNT; ++i)
	{
		SAFE_DELETE(myMovements[i]);
	}
}

void MovementComponent::Reset()
{
	for (int i = 0; i < eMovementType::_COUNT; ++i)
	{
		myMovements[i]->Reset();
	}

	myDashCooldown = 0.f;
}

void MovementComponent::Update(float aDeltaTime)
{
	myDashCooldown -= aDeltaTime;
	myMovements[myCurrentMovement]->Update(aDeltaTime);

	if (myIsInSteam == true)
	{
		myMovements[myCurrentMovement]->Impulse(mySteamVelocity * aDeltaTime);
	}
}

void MovementComponent::ReceiveNote(const ContactNote& aNote)
{
	myMovements[myCurrentMovement]->ReceiveNote(aNote);
}

void MovementComponent::Impulse()
{
	myMovements[myCurrentMovement]->Impulse();
}

void MovementComponent::Impulse(const CU::Vector2<float>& aVelocity)
{
	myMovements[myCurrentMovement]->Impulse(aVelocity);
}

void MovementComponent::SetDirectionTarget(const CU::Vector2<float>& aDirection)
{
	myMovements[myCurrentMovement]->SetDirectionTarget(aDirection);
}

void MovementComponent::RightTriggerDown()
{
	if (myDashCooldown <= 0.f)
	{
		SetState(eMovementType::DASH_AIM);
	}
}

void MovementComponent::RightTriggerUp()
{
	if (myCurrentMovement == eMovementType::DASH_AIM)
	{
		SetState(eMovementType::DASH_FLY);
	}
}

void MovementComponent::SetState(eMovementType aState)
{
	if (myCurrentMovement == eMovementType::DASH_AIM)
	{
		myDashCooldown = myData.myDashCooldown;
	}
	myMovements[myCurrentMovement]->DeActivate();
	myCurrentMovement = aState;
	myMovements[myCurrentMovement]->Activate();
}

