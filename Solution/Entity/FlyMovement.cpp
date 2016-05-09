#include "stdafx.h"
#include "ContactNote.h"
#include "FlyMovement.h"
#include "MovementComponent.h"
#include <PhysicsInterface.h>

FlyMovement::FlyMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation, MovementComponent& aMovementComponent)
	: Movement(aData, anOrientation, aMovementComponent)
	, myHasContact(false)
{
}


FlyMovement::~FlyMovement()
{
}

void FlyMovement::Reset()
{
	myVelocity = CU::Vector2<float>();
}

void FlyMovement::Update(float aDeltaTime)
{
	HandleContact();

	myVelocity.y += myData.myGravity * aDeltaTime;
	Drag(aDeltaTime);
	Rotate(aDeltaTime);

	Translate();
}

void FlyMovement::SetDirectionTarget(const CU::Vector2<float>& aDirection)
{
	myDirectionTarget = aDirection;
}

void FlyMovement::Impulse()
{
	myVelocity += CU::Vector3<float>(CU::Vector3<float>(0, myData.myImpulse, 0) * myOrientation).GetVector2();
}

void FlyMovement::Impulse(const CU::Vector2<float>& aVelocity)
{
	myVelocity += aVelocity;
}

void FlyMovement::Activate()
{
	myVelocity.x = 0.f;
	myVelocity.y = 0.f;
	myIsActive = true;
}

void FlyMovement::DeActivate()
{
	myIsActive = false;
}

void FlyMovement::SetVelocity(const CU::Vector2<float>& aVelocity)
{ 
	myVelocity = aVelocity;
}

void FlyMovement::ReceiveNote(const ContactNote&)
{
}

void FlyMovement::HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection
	, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
{
	if (myIsActive == false) return;
	if (aComponent != nullptr)
	{
		myHasContact = true;
		
		CU::Vector3<float> resetPos(myOrientation.GetPos());
		resetPos.z = 0.f;

		if (aHitNormal.y > 0.f)
		{
			if (aComponent->GetEntity().GetType() != eEntityType::BOUNCER)
			{
				myMovementComponent.SetState(MovementComponent::eMovementType::WALK);
			}
		}
		else if (aHitNormal.y < 0.f)
		{
			resetPos.y = aHitPosition.y - GC::PlayerRadius * 1.1f;
			myVelocity.y = 0;
		}
		else if (aDirection.x > 0)
		{
			resetPos.x = aHitPosition.x - GC::PlayerRadius* 1.1f;
			myVelocity.x = 0;
		}
		else if (aDirection.x < 0)
		{
			resetPos.x = aHitPosition.x + GC::PlayerRadius* 1.1f;
			myVelocity.x = 0;
		}
		myOrientation.SetPos(resetPos);
	}
}

void FlyMovement::HandleContact()
{
	CU::Vector3<float> leftOrigin(myOrientation.GetPos().x - GC::PlayerRadius, myOrientation.GetPos().y, 0.f);
	CU::Vector3<float> rightOrigin(myOrientation.GetPos().x + GC::PlayerRadius, myOrientation.GetPos().y, 0.f);
	CU::Vector3<float> centerPosition(myOrientation.GetPos());

	CU::Vector3<float> down(0.f, -1.f, 0.f);
	CU::Vector3<float> up(0.f, 1.f, 0.f);

	Prism::PhysicsInterface::GetInstance()->RayCast(leftOrigin, down, GC::PlayerHeightWithLegs, myRaycastHandler
		, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());
	Prism::PhysicsInterface::GetInstance()->RayCast(rightOrigin, down, GC::PlayerHeightWithLegs, myRaycastHandler
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

void FlyMovement::Drag(float aDeltaTime)
{
	if (myVelocity.x > 0)
	{
		myVelocity.x = fmaxf(myVelocity.x - myData.myDrag.x * aDeltaTime, 0);
	}
	else if (myVelocity.x < 0)
	{
		myVelocity.x = fminf(myVelocity.x + myData.myDrag.x * aDeltaTime, 0);
	}
	if (myVelocity.y > 0)
	{
		myVelocity.y = fmaxf(myVelocity.y - myData.myDrag.y * aDeltaTime, 0);
	}
	else if (myVelocity.y < 0)
	{
		myVelocity.y = fminf(myVelocity.y + myData.myDrag.y * aDeltaTime, 0);
	}
}



void FlyMovement::Rotate(float aDeltaTime)
{
	if (CU::Length2(myDirectionTarget) > myData.myDeadZone * myData.myDeadZone)
	{
		CU::Vector2<float> up(myOrientation.GetUp().GetVector2());
		CU::Vector2<float> target(CU::GetNormalized(myDirectionTarget));

		float dot = CU::Dot(up, target);
		if (acosf(dot) > myData.myAngleEpsilon)
		{
			float perpDot = up.x * target.y - up.y * target.x;

			float signedAngle = atan2f(perpDot, dot);

			if (signedAngle < 0)
			{
				myOrientation = CU::Matrix44<float>::CreateRotateAroundZ(-myData.myRotationSpeed * aDeltaTime) * myOrientation;
			}
			else if (signedAngle > 0)
			{
				myOrientation = CU::Matrix44<float>::CreateRotateAroundZ(myData.myRotationSpeed * aDeltaTime) * myOrientation;
			}
		}
	}
}

void FlyMovement::Translate()
{
	myOrientation.SetPos(myOrientation.GetPos() + CU::Vector3<float>(myVelocity, 0));

	//myOrientation.SetPos(CU::Vector3<float>(myOrientation.GetPos().x, fmaxf(myOrientation.GetPos().y, 0), myOrientation.GetPos().z));

	//if (myOrientation.GetPos().y == 0)
	//{
	//	myVelocity.y = 0;
	//}


	//only for debugging, keeping player inside screen:
	//if (myOrientation.GetPos().x < -15.f)
	//{
	//	myVelocity.x = fmaxf(myVelocity.x, 0);
	//}
	//else if (myOrientation.GetPos().x > 15.f)
	//{
	//	myVelocity.x = fminf(myVelocity.x, 0);
	//}
	//debugging out
}