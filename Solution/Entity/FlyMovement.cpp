#include "stdafx.h"
#include "ContactNote.h"
#include "FlyMovement.h"
#include "MovementComponent.h"
#include "InputComponent.h"
#include "Entity.h"
#include "LoseBodyPartNote.h"
#include "PlayerComponent.h"
#include "PlayerComponentData.h"
#include "PlayerGraphicsComponent.h"
#include "PhysicsComponent.h"
#include <PhysicsInterface.h>
#include <PostMaster.h>

FlyMovement::FlyMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation, MovementComponent& aMovementComponent)
	: Movement(aData, anOrientation, aMovementComponent)
	, myHasContact(false)
	, myPlayerData(nullptr)
{
	myRaycastHandlerHead = [=](PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
	{
		this->HandleRaycastHead(aComponent, aDirection, aHitPosition, aHitNormal);
	};

	myRaycastHandlerLegs = [=](PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
	{
		this->HandleRaycastLegs(aComponent, aDirection, aHitPosition, aHitNormal);
	};
}


FlyMovement::~FlyMovement()
{
}

void FlyMovement::Init()
{
	myPlayerData = myMovementComponent.GetEntity().GetComponent<PlayerComponent>()->GetData();
}

void FlyMovement::Reset()
{
	myVelocity = CU::Vector2<float>();
}

void FlyMovement::Update(float aDeltaTime)
{
	RaycastBody();
	RaycastHead();
	RaycastLegs();

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

void FlyMovement::Activate(const CU::Vector2<float>& aVelocity)
{
	myVelocity = aVelocity;
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
		myMovementComponent.GetEntity().GetComponent<PlayerComponent>()->HandleCollision(&aComponent->GetEntity());

		const eEntityType& type = aComponent->GetEntity().GetType();
		if (type == eEntityType::SAW_BLADE || type == eEntityType::SPIKE || type == eEntityType::SCRAP) return;
		if (type == eEntityType::BOUNCER)
		{
			float dot = CU::Dot(aHitNormal, aComponent->GetEntity().GetOrientation().GetUp());

			if (dot > 0.001f)
			{
				return;
			}
		}
		myHasContact = true;

		CU::Vector3<float> resetPos(myOrientation.GetPos());
		resetPos.z = 0.f;

		if (aHitNormal.y > 0.5f)
		{
			if (aComponent->GetEntity().GetType() != eEntityType::BOUNCER)
			{
				if (myIsInSteam == false)
				{
					resetPos.y = aHitPosition.y + GC::PlayerRadius * 1.f;
					myMovementComponent.SetState(MovementComponent::eMovementType::WALK, myVelocity);
				}
				else if (myVelocity.y < 0.f)
				{
					myVelocity.y = 0;
					resetPos.y = aHitPosition.y + GC::PlayerRadius * 1.f;
				}
			}
		}
		else if (aHitNormal.y < -0.5f)
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

void FlyMovement::HandleRaycastHead(PhysicsComponent* aComponent, const CU::Vector3<float>&
	, const CU::Vector3<float>&, const CU::Vector3<float>&)
{
	if (aComponent != nullptr && CU::Length2(myVelocity) > myPlayerData->myLoseHeadSpeed * myPlayerData->myLoseHeadSpeed)
	{
		if (aComponent->GetEntity().GetType() == eEntityType::SCRAP
			|| aComponent->GetEntity().GetType() == eEntityType::BOUNCER)
		{
			myMovementComponent.GetEntity().SendNote(LoseBodyPartNote(eScrapPart::HEAD));
		}
	}
}

void FlyMovement::HandleRaycastLegs(PhysicsComponent* aComponent, const CU::Vector3<float>&
	, const CU::Vector3<float>&, const CU::Vector3<float>&)
{
	if (aComponent != nullptr && CU::Length2(myVelocity) > myPlayerData->myLoseLegsSpeed * myPlayerData->myLoseLegsSpeed)
	{
		if (aComponent->GetEntity().GetType() != eEntityType::SCRAP)
		{
			myMovementComponent.GetEntity().SendNote(LoseBodyPartNote(eScrapPart::LEGS));
		}
	}
}

void FlyMovement::RaycastBody()
{
	CU::Vector3<float> leftOrigin(myOrientation.GetPos().x - GC::PlayerRadius, myOrientation.GetPos().y, 0.f);
	CU::Vector3<float> rightOrigin(myOrientation.GetPos().x + GC::PlayerRadius, myOrientation.GetPos().y, 0.f);
	CU::Vector3<float> centerPosition(myOrientation.GetPos());

	CU::Vector3<float> down(0.f, -1.f, 0.f);
	CU::Vector3<float> up(0.f, 1.f, 0.f);

	float raycastLengthWithLegs = GC::PlayerHeightWithLegs + 0.05f;
	if (myMovementComponent.GetEntity().GetComponent<PlayerGraphicsComponent>()->GetLegsActive() == false)
	{
		raycastLengthWithLegs *= 0.8f;
	}

	Prism::PhysicsInterface::GetInstance()->RayCast(leftOrigin, down, raycastLengthWithLegs, myRaycastHandler
		, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());
	Prism::PhysicsInterface::GetInstance()->RayCast(rightOrigin, down, raycastLengthWithLegs, myRaycastHandler
		, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());

	Prism::PhysicsInterface::GetInstance()->RayCast(leftOrigin, up, GC::PlayerHeightWithLegs + 0.025f, myRaycastHandler
		, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());
	Prism::PhysicsInterface::GetInstance()->RayCast(rightOrigin, up, GC::PlayerHeightWithLegs + 0.025f, myRaycastHandler
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

void FlyMovement::RaycastHead()
{
	Prism::PhysicsInterface::GetInstance()->RayCast(myOrientation.GetPos(), myOrientation.GetUp(), myPlayerData->myHeadDistance, myRaycastHandlerHead
		, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());
}

void FlyMovement::RaycastLegs()
{
	Prism::PhysicsInterface::GetInstance()->RayCast(myOrientation.GetPos(), myOrientation.GetUp() * -1.f, myPlayerData->myLegsDistance, myRaycastHandlerLegs
		, myMovementComponent.GetEntity().GetComponent<PhysicsComponent>());
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

			if (myMovementComponent.GetEntity().GetComponent<InputComponent>()->GetIsFlipped())
			{
				signedAngle *= -1.f;
			}

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