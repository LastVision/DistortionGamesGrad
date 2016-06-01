#include "stdafx.h"

#include <Bar3D.h>
#include "DashAimMovement.h"
#include "Entity.h"
#include <Effect.h>
#include <EffectContainer.h>
#include <EmitterMessage.h>
#include <ModelLoader.h>
#include "MovementComponent.h"
#include <Instance.h>
#include "InputComponent.h"
#include <Scene.h>
#include <TextureContainer.h>
#include <PostMaster.h>

DashAimMovement::DashAimMovement(const MovementComponentData& aData, CU::Matrix44f& anOrientation
	, MovementComponent& aMovementComponent, Prism::Scene* aScene)
	: Movement(aData, anOrientation, aMovementComponent)
	, myTimer(aData.myDashAimTime)
	, myScene(aScene)
{
	Prism::ModelLoader::GetInstance()->Pause();
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/SM_arrow.fbx", "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myArrow = new Prism::Instance(*model, myOrientation);

	myScene->AddInstance(myArrow, true);
	myArrow->SetShouldRender(false);


	myEffect = Prism::EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_3dgui.fx");

	myAimTimer = new Prism::Bar3D(0.5f, 1.f, 64, myEffect, eBarPosition::AIM, "Data/Resource/Texture/T_aimbar.dds");
	Prism::ModelLoader::GetInstance()->UnPause();
}


DashAimMovement::~DashAimMovement()
{
	SAFE_DELETE(myArrow);
	SAFE_DELETE(myAimTimer);
}

void DashAimMovement::Reset()
{
	myVelocity = CU::Vector2<float>();
	myIsActive = false;
	myIsInSteam = false;
}

void DashAimMovement::Update(float aDeltaTime, bool)
{
	Rotate(aDeltaTime);

	myAimTimer->SetValue(myTimer / myData.myDashAimTime);

	myTimer -= aDeltaTime;
	if (myTimer <= 0.f)
	{
		myMovementComponent.SetState(MovementComponent::eMovementType::DASH_FLY, myVelocity);
		//PostMaster::GetInstance()->SendMessage(EmitterMessage("Dash_Recharge", myOrientation.GetPos(), myData.myDashAimTime)); Should be a streak thingy
	}
}

void DashAimMovement::Render() 
{
	myAimTimer->Render(*myScene->GetCamera(), myMovementComponent.GetEntity().GetOrientation());
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

void DashAimMovement::Activate(const CU::Vector2<float>&)
{
	myTimer = myData.myDashAimTime;
	myArrow->SetShouldRender(true);
	myIsActive = true;
}

void DashAimMovement::DeActivate()
{
	myArrow->SetShouldRender(false);
	myIsActive = false;
}

void DashAimMovement::SetVelocity(const CU::Vector2<float>&)
{
}

void DashAimMovement::ReceiveNote(const ContactNote&)
{
	//Not needed here?
}

void DashAimMovement::HandleRaycast(PhysicsComponent*, const CU::Vector3<float>&
	, const CU::Vector3<float>&, const CU::Vector3<float>&)
{

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
