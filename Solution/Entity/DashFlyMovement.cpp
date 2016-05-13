#include "stdafx.h"
#include "DashFlyMovement.h"
#include "InputComponent.h"
#include "MovementComponent.h"
#include "PhysicsComponent.h"
#include <PhysicsInterface.h>
#include "ShouldDieNote.h"

DashFlyMovement::DashFlyMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation, MovementComponent& aMovementComponent)
	: Movement(aData, anOrientation, aMovementComponent)
	, myTimer(aData.myDashFlyTime)
	, myHasContact(false)
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
	HandleContact();
	myOrientation.SetPos(myOrientation.GetPos() + CU::Vector3<float>(myVelocity*aDeltaTime, 0));

	myTimer -= aDeltaTime;
	if (myTimer <= 0.f)
	{
		myMovementComponent.SetState(MovementComponent::eMovementType::FLY, (myVelocity * 0.3f));
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

void DashFlyMovement::Activate(const CU::Vector2<float>&)
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
	if (myIsActive == false) return;
	if (aComponent != nullptr)
	{
		eEntityType type = aComponent->GetEntity().GetType();
		if (type == eEntityType::SCRAP) return;

		myHasContact = true;
		if (type != eEntityType::BOUNCER)
		{
			myMovementComponent.GetEntity().SendNote(ShouldDieNote());
		}
		else if (type == eEntityType::PLAYER)
		{
			aComponent->GetEntity().SendNote(ShouldDieNote());
		}
		else
		{
			myMovementComponent.SetState(MovementComponent::eMovementType::FLY, (myVelocity * 0.3f));
		}
	}
}

void DashFlyMovement::HandleContact()
{
	CU::Vector3<float> leftOrigin(myOrientation.GetPos().x - GC::PlayerRadius, myOrientation.GetPos().y, 0.f);
	CU::Vector3<float> rightOrigin(myOrientation.GetPos().x + GC::PlayerRadius, myOrientation.GetPos().y, 0.f);
	CU::Vector3<float> centerPosition(myOrientation.GetPos());

	CU::Vector3<float> down(0.f, -1.f, 0.f);
	CU::Vector3<float> up(0.f, 1.f, 0.f);

	Prism::PhysicsInterface::GetInstance()->RayCast(leftOrigin, down, GC::PlayerHeightWithLegs * 0.8f, myRaycastHandler
		, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());
	Prism::PhysicsInterface::GetInstance()->RayCast(rightOrigin, down, GC::PlayerHeightWithLegs* 0.8f, myRaycastHandler
		, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());

	Prism::PhysicsInterface::GetInstance()->RayCast(leftOrigin, up, GC::PlayerHeightWithLegs, myRaycastHandler
		, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());
	Prism::PhysicsInterface::GetInstance()->RayCast(rightOrigin, up, GC::PlayerHeightWithLegs, myRaycastHandler
		, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());



	CU::Vector3<float> left(-1.f, 0.f, 0.f);
	CU::Vector3<float> right(1.f, 0.f, 0.f);

	if (myVelocity.x < 0)
	{
		Prism::PhysicsInterface::GetInstance()->RayCast(centerPosition, left, GC::PlayerRadius, myRaycastHandler
			, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());
	}
	else if (myVelocity.x > 0)
	{
		Prism::PhysicsInterface::GetInstance()->RayCast(centerPosition, right, GC::PlayerRadius, myRaycastHandler
			, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());
	}


	myHasContact = false;
}
