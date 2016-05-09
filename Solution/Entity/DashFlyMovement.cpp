#include "stdafx.h"
#include "DashFlyMovement.h"
#include "MovementComponent.h"

DashFlyMovement::DashFlyMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation, MovementComponent& aMovementComponent)
	: Movement(aData, anOrientation, aMovementComponent)
	, myTimer(aData.myDashFlyTime)
{
}


DashFlyMovement::~DashFlyMovement()
{
}

void DashFlyMovement::Reset()
{
	myTimer = myData.myDashFlyTime;
}

void DashFlyMovement::Update(float aDeltaTime)
{
	myOrientation.SetPos(myOrientation.GetPos() + CU::Vector3<float>(myVelocity*aDeltaTime, 0));

	myTimer -= aDeltaTime;
	if (myTimer <= 0.f)
	{
		myMovementComponent.SetState(MovementComponent::eMovementType::FLY);
	}
}

void DashFlyMovement::ReceiveNote(const ContactNote&)
{
}

void DashFlyMovement::SetDirectionTarget(const CU::Vector2<float>&)
{
	//Not needed here?
}

void DashFlyMovement::Impulse()
{
	//Not needed here?
}

void DashFlyMovement::Impulse(const CU::Vector2<float>&)
{
}

void DashFlyMovement::Activate()
{
	myVelocity = myOrientation.GetRight().GetVector2() * myData.myDashSpeed;
	myTimer = myData.myDashFlyTime;
	myIsActive = true;
}

void DashFlyMovement::DeActivate()
{
	myIsActive = false;
}

void DashFlyMovement::SetVelocity(const CU::Vector2<float>&)
{
}

void DashFlyMovement::HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection
	, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
{

}
