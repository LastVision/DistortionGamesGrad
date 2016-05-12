#include "stdafx.h"

#include "DeathNote.h"
#include "InputComponent.h"
#include "MovementComponent.h"
#include <OnDeathMessage.h>
#include <PlayerActiveMessage.h>
#include "PlayerComponent.h"
#include <PostMaster.h>
#include "ShouldDieNote.h"

PlayerComponent::PlayerComponent(Entity& anEntity)
	: Component(anEntity)
	, myShouldDie(false)
{
}

PlayerComponent::~PlayerComponent()
{
}

void PlayerComponent::Update(float)
{
}

void PlayerComponent::EvaluateDeath()
{
	if (myShouldDie == true)
	{
		myShouldDie = false;

		PostMaster::GetInstance()->SendMessage(OnDeathMessage(myEntity.GetComponent<InputComponent>()->GetPlayerID()));
		myEntity.Reset();
		myEntity.SendNote(DeathNote());
	}
}

void PlayerComponent::HandleCollision(Entity* aOther)
{
	if (aOther->GetType() == eEntityType::BOUNCER)
	{
		return;
	}

	CU::Vector2<float> velocity = myEntity.GetComponent<MovementComponent>()->GetVelocity();
	if (CU::Length2(velocity) > 0.2f*0.2f)
	{
		myEntity.SendNote(ShouldDieNote());
	}
}

void PlayerComponent::ReceiveNote(const ShouldDieNote&)
{
	myShouldDie = true;
}

void PlayerComponent::ReceiveNote(const SpawnNote&)
{
	PostMaster::GetInstance()->SendMessage(PlayerActiveMessage(true, myEntity.GetComponent<InputComponent>()->GetPlayerID()));
}