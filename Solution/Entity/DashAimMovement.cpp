#include "stdafx.h"
#include "DashAimMovement.h"
#include "MovementComponent.h"

DashAimMovement::DashAimMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation, MovementComponent& aMovementComponent)
	: Movement(aData, anOrientation, aMovementComponent)
	, myTimer(aData.myDashAimTime)
{
}


DashAimMovement::~DashAimMovement()
{
}

void DashAimMovement::Reset()
{

}

void DashAimMovement::Update(float aDeltaTime)
{
	Rotate(aDeltaTime);

	myTimer -= aDeltaTime;
	if (myTimer <= 0.f)
	{
		myMovementComponent.SetState(MovementComponent::eMovementType::DASH_FLY);
	}
}

void DashAimMovement::SetDirectionTarget(const CU::Vector2<float>& aDirection)
{
	myDirectionTarget = aDirection;
}

void DashAimMovement::Impulse()
{
}

void DashAimMovement::Activate()
{
	myTimer = myData.myDashAimTime;
}

void DashAimMovement::ReceiveNote(const ContactNote&)
{
	//Not needed here?
}

void DashAimMovement::Rotate(float aDeltaTime)
{
	if (CU::Length2(myDirectionTarget) > myData.myDeadZone * myData.myDeadZone)
	{
		CU::Vector2<float> right(myOrientation.GetRight().GetVector2());
		CU::Vector2<float> target(CU::GetNormalized(myDirectionTarget));

		float dot = CU::Dot(right, target);
		if (acosf(dot) > myData.myAngleEpsilon)
		{
			float perpDot = right.x * target.y - right.y * target.x;

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
