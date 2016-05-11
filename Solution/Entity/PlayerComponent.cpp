#include "stdafx.h"

#include "DeathNote.h"
#include "InputComponent.h"
#include <OnDeathMessage.h>
#include <PlayerActiveMessage.h>
#include "PlayerComponent.h"
#include <PostMaster.h>

PlayerComponent::PlayerComponent(Entity& anEntity)
	: Component(anEntity)
	, myShouldDie(false)
{
}

PlayerComponent::~PlayerComponent()
{
}

void PlayerComponent::Update(float aDeltaTime)
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

void PlayerComponent::ReceiveNote(const ShouldDieNote& aMessage)
{
	myShouldDie = true;
}

void PlayerComponent::ReceiveNote(const SpawnNote& aMessage)
{
	PostMaster::GetInstance()->SendMessage(PlayerActiveMessage(true, myEntity.GetComponent<InputComponent>()->GetPlayerID()));
}