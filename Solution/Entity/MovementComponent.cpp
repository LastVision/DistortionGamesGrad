#include "stdafx.h"
#include "DashAimMovement.h"
#include "DashFlyMovement.h"
#include "MovementComponent.h"
#include "FlyMovement.h"
#include "WalkMovement.h"

MovementComponent::MovementComponent(Entity& aEntity, const MovementComponentData& aData, CU::Matrix44f& anOrientation)
	: Component(aEntity)
	, myData(aData)
	, myCurrentMovement(eMovementType::FLY)
	, myDashCooldown(myData.myDashCooldown)
{
	myMovements[eMovementType::FLY] = new FlyMovement(aData, anOrientation, *this);
	myMovements[eMovementType::WALK] = new WalkMovement(aData, anOrientation, *this);
	myMovements[eMovementType::DASH_AIM] = new DashAimMovement(aData, anOrientation, *this);
	myMovements[eMovementType::DASH_FLY] = new DashFlyMovement(aData, anOrientation, *this);
}

MovementComponent::~MovementComponent()
{
}

void MovementComponent::Reset()
{
	for (int i = 0; i < eMovementType::_COUNT; ++i)
	{
		myMovements[i]->Reset();
	}
}

void MovementComponent::Update(float aDeltaTime)
{
	myDashCooldown -= aDeltaTime;
	myMovements[myCurrentMovement]->Update(aDeltaTime);
}

void MovementComponent::ReceiveNote(const ContactNote& aNote)
{
	//CU::Vector2<float> toContact(aNote.myContactPoint.GetVector2() - myOrientation.GetPos().GetVector2());
	//if (aNote.myContactPoint.GetVector2().x > 0 || aNote.myContactPoint.GetVector2().x < 0)
	//{
	//	myVelocity.x = 0;
	//}
	//if (aNote.myContactPoint.GetVector2().y > 0 || aNote.myContactPoint.GetVector2().y < 0)
	//{
	//	myVelocity.y = 0;
	//}
}

void MovementComponent::Impulse()
{
	myMovements[myCurrentMovement]->Impulse();
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

	myCurrentMovement = aState;
	myMovements[myCurrentMovement]->Activate();
}

