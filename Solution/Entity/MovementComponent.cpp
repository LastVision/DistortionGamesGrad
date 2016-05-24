#include "stdafx.h"

#include <AudioInterface.h>
#include "DashAimMovement.h"
#include "DashFlyMovement.h"
#include "FlyMovement.h"
#include "InputComponent.h"
#include "MovementComponent.h"
#include "PostMaster.h"
#include "PlayerActiveMessage.h"
#include "SoundComponent.h"
#include "WalkMovement.h"
#include <PostMaster.h>
#include <EmitterMessage.h>

MovementComponent::MovementComponent(Entity& aEntity, const MovementComponentData& aData, CU::Matrix44f& anOrientation, Prism::Scene* aScene)
	: Component(aEntity)
	, myData(aData)
	, myCurrentMovement(eMovementType::FLY)
	, myDashCooldown(0.f)
	, myDeltaTime(0.f)
{
	myMovements[eMovementType::FLY] = new FlyMovement(aData, anOrientation, *this);
	myMovements[eMovementType::WALK] = new WalkMovement(aData, anOrientation, *this);
	myMovements[eMovementType::DASH_AIM] = new DashAimMovement(aData, anOrientation, *this, aScene);
	myMovements[eMovementType::DASH_FLY] = new DashFlyMovement(aData, anOrientation, *this);
	myMovements[myCurrentMovement]->Activate({ 0.f, 0.f });

	PostMaster::GetInstance()->Subscribe(this, eMessageType::PLAYER_ACTIVE);
}

MovementComponent::~MovementComponent()
{
	for (int i = 0; i < eMovementType::_COUNT; ++i)
	{
		SAFE_DELETE(myMovements[i]);
	}

	PostMaster::GetInstance()->UnSubscribe(this, 0);
}

void MovementComponent::Init()
{
	for (int i = 0; i < eMovementType::_COUNT; ++i)
	{
		myMovements[i]->Init();
	}
}

void MovementComponent::Reset()
{
	for (int i = 0; i < eMovementType::_COUNT; ++i)
	{
		myMovements[i]->Reset();
	}
	myDashCooldown = 0.f;
}

void MovementComponent::Update(float aDeltaTime)
{
	DEBUG_PRINT(myEntity.GetOrientation().GetPos());
	DEBUG_PRINT(CU::Length(GetVelocity()));

	myDeltaTime = aDeltaTime;
	myDashCooldown -= aDeltaTime;
	myMovements[myCurrentMovement]->Update(aDeltaTime);

	if (myIsInSteam == true)
	{
		myMovements[myCurrentMovement]->Impulse(mySteamVelocity * aDeltaTime);
	}
}

void MovementComponent::Render()
{
	myMovements[myCurrentMovement]->Render();
}

void MovementComponent::ReceiveNote(const ContactNote& aNote)
{
	myMovements[myCurrentMovement]->ReceiveNote(aNote);
}

void MovementComponent::Impulse()
{
	myMovements[myCurrentMovement]->Impulse();
	if (myCurrentMovement == eMovementType::FLY
		|| myCurrentMovement == eMovementType::WALK)
	{
		SoundComponent* soundComp = myEntity.GetComponent<SoundComponent>();
		if (soundComp != nullptr)
		{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Impulse", soundComp->GetAudioSFXID());
		}
	}
}

void MovementComponent::Impulse(const CU::Vector2<float>& aVelocity)
{
	myMovements[myCurrentMovement]->Impulse(aVelocity);
}

void MovementComponent::SetDirectionTarget(const CU::Vector2<float>& aDirection)
{
	myMovements[myCurrentMovement]->SetDirectionTarget(aDirection);
}

void MovementComponent::RightTriggerDown()
{
	if (myDashCooldown <= 0.f)
	{
		SetState(eMovementType::DASH_AIM, { 0.f, 0.f });
	}
}

void MovementComponent::RightTriggerUp()
{
	if (myCurrentMovement == eMovementType::DASH_AIM)
	{
		SetState(eMovementType::DASH_FLY, { 0.f, 0.f });
	}
}

void MovementComponent::SetState(eMovementType aState, const CU::Vector2<float>& aVelocity)
{
	if (myCurrentMovement == eMovementType::DASH_AIM)
	{
		myDashCooldown = myData.myDashCooldown;
		PostMaster::GetInstance()->SendMessage(EmitterMessage("Dash_Recharge", &myEntity, myDashCooldown));
	}
	myMovements[myCurrentMovement]->DeActivate();
	CU::Vector2<float> velocityToState(aVelocity);
	if (myCurrentMovement != eMovementType::WALK)
	{
		velocityToState *= myDeltaTime;
	}
	myCurrentMovement = aState;
	myMovements[myCurrentMovement]->Activate(velocityToState);
}

void MovementComponent::SetInSteam(bool aIsInSteam, const CU::Vector2<float>& aVelocity)
{
	myIsInSteam = aIsInSteam;
	mySteamVelocity = aVelocity;

	for (int i = 0; i < eMovementType::_COUNT; ++i)
	{
		myMovements[i]->SetIsInSteam(aIsInSteam);
	}
}

void MovementComponent::SetVelocity(const CU::Vector2<float>& aVelocity)
{
	myMovements[myCurrentMovement]->SetVelocity(aVelocity);
}

const CU::Vector2<float>& MovementComponent::GetVelocity() const
{
	return myMovements[myCurrentMovement]->GetVelocity();
}

void MovementComponent::ReceiveNote(const DeathNote&)
{
	for (int i = 0; i < eMovementType::_COUNT; ++i)
	{
		myMovements[i]->DeActivate();
	}
}

void MovementComponent::ReceiveNote(const SpawnNote&)
{
	myCurrentMovement = eMovementType::FLY;
	//mySpawnVelocity = { 0.05f, 0.01f };
	myMovements[myCurrentMovement]->Activate(mySpawnVelocity);
	//myEntity.ResetPosition();
}