#include "stdafx.h"
#include "MovementComponent.h"
#include "FlyMovement.h"
#include "WalkMovement.h"

MovementComponent::MovementComponent(Entity& aEntity, const MovementComponentData& aData, CU::Matrix44f& anOrientation)
	: Component(aEntity)
	, myData(aData)
	, myCurrentMovement(eMovementType::FLY)
{
	myMovements[eMovementType::FLY] = new FlyMovement(aData, anOrientation);
	myMovements[eMovementType::WALK] = new WalkMovement(aData, anOrientation);
	myMovements[eMovementType::DASH_AIM] = new FlyMovement(aData, anOrientation);
	myMovements[eMovementType::DASH_FLY] = new FlyMovement(aData, anOrientation);
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

