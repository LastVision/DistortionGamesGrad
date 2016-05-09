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
	if (HandleContact() == true) return;

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
	if (aComponent != nullptr && aHitNormal.y > 0.f)
	{
		myHasContact = true;
		myMovementComponent.SetState(MovementComponent::eMovementType::WALK);
	}
}

bool FlyMovement::HandleContact()
{
	CU::Vector3<float> leftOrigin(myOrientation.GetPos().x - 0.5f, myOrientation.GetPos().y, 0.f);
	CU::Vector3<float> rightOrigin(myOrientation.GetPos().x + 0.5f, myOrientation.GetPos().y, 0.f);

	CU::Vector3<float> dir(0.f, -1.f, 0.f);

	Prism::PhysicsInterface::GetInstance()->RayCast(leftOrigin, dir, GC::PlayerHeightWithLegs, myRaycastHandler
		, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());


	Prism::PhysicsInterface::GetInstance()->RayCast(rightOrigin, dir, GC::PlayerHeightWithLegs, myRaycastHandler
		, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());

	myHasContact = false;

	return false;
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