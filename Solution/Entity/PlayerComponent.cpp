#include "stdafx.h"
#include "InputComponent.h"
#include <OnDeathMessage.h>
#include <PlayerActiveMessage.h>
#include "PlayerComponent.h"
#include <PostMaster.h>

PlayerComponent::PlayerComponent(Entity& anEntity)
	: Component(anEntity)
{
}

PlayerComponent::~PlayerComponent()
{
}

void PlayerComponent::Update(float aDeltaTime)
{
}

void PlayerComponent::ReceiveNote(const DeathNote& aMessage)
{
	PostMaster::GetInstance()->SendMessage(OnDeathMessage(myEntity.GetComponent<InputComponent>()->GetPlayerID()));
}

void PlayerComponent::ReceiveNote(const SpawnNote& aMessage)
{
	PostMaster::GetInstance()->SendMessage(PlayerActiveMessage(true, myEntity.GetComponent<InputComponent>()->GetPlayerID()));
}