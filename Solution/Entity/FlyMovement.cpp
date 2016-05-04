#include "stdafx.h"
#include "FlyMovement.h"
#include "ContactNote.h"

FlyMovement::FlyMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation)
	: Movement(aData, anOrientation)
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

	myPreviousPosition = myOrientation.GetPos().GetVector2();
}

void FlyMovement::SetDirectionTarget(const CU::Vector2<float>& aDirection)
{
	myDirectionTarget = aDirection;
}

void FlyMovement::Impulse()
{
	myVelocity += CU::Vector3<float>(CU::Vector3<float>(0, myData.myImpulse, 0) * myOrientation).GetVector2();
}

void FlyMovement::ReceiveNote(const ContactNote& aNote)
{
	myContact.myOther = aNote.myOther;
	myContact.myContactPoint.x = aNote.myContactPoint.x;
	myContact.myContactPoint.y = aNote.myContactPoint.y;
	myContact.myContactNormal.x = aNote.myContactNormal.x;
	myContact.myContactNormal.y = aNote.myContactNormal.y;
	myContact.myActive = true;
}

void FlyMovement::HandleContact()
{
	if (myContact.myActive == true)
	{
		//CU::Vector3<float> moveAmount;
		//myOrientation.SetPos()
	}
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