#include "stdafx.h"
#include "DashAimMovement.h"
#include <ModelLoader.h>
#include "MovementComponent.h"
#include <Instance.h>
#include <Scene.h>

DashAimMovement::DashAimMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation
	, MovementComponent& aMovementComponent, Prism::Scene* aScene)
	: Movement(aData, anOrientation, aMovementComponent)
	, myTimer(aData.myDashAimTime)
	, myScene(aScene)
{
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/SM_arrow.fbx", "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myArrow = new Prism::Instance(*model, myOrientation);

	myScene->AddInstance(myArrow);
	myArrow->SetShouldRender(false);
}


DashAimMovement::~DashAimMovement()
{
	SAFE_DELETE(myArrow);
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

void DashAimMovement::Impulse(const CU::Vector2<float>&)
{
}

void DashAimMovement::Activate()
{
	myTimer = myData.myDashAimTime;
	myArrow->SetShouldRender(true);
}

void DashAimMovement::DeActivate()
{
	myArrow->SetShouldRender(false);
}

void DashAimMovement::SetVelocity(const CU::Vector2<float>&)
{
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
