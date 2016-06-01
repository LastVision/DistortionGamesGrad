#include "stdafx.h"

#include "DeathNote.h"
#include "InputComponent.h"
#include "MovementComponent.h"
#include <OnDeathMessage.h>
#include "PhysicsComponent.h"
#include <PhysicsInterface.h>
#include <PlayerActiveMessage.h>
#include "PlayerComponent.h"
#include <PostMaster.h>
#include "PlayerGraphicsComponent.h"
#include "ShouldDieNote.h"
#include <ScrapMessage.h>

PlayerComponent::PlayerComponent(Entity& anEntity, const PlayerComponentData& aData)
	: Component(anEntity)
	, myData(aData)
	, myShouldDie(false)
{
	myRaycastHandler = [=](PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
	{
		this->HandleRaycast(aComponent, aDirection, aHitPosition, aHitNormal);
	};
}

PlayerComponent::~PlayerComponent()
{
}

void PlayerComponent::Reset()
{
	myPreviousPosition = myEntity.GetOrientation().GetPos();
}

void PlayerComponent::Update(float)
{
	CU::Vector3<float> position(myEntity.GetOrientation().GetPos());
	if (position.x < 0.f || position.x > 70.f || position.y < 0.f || position.y > 36.f)
	{
		myEntity.SendNote(ShouldDieNote());
	}

	CU::Vector3<float> direction(myEntity.GetOrientation().GetPos() - myPreviousPosition);

	if (direction != CU::Vector3<float>())
	{
		float length = CU::Length(direction);
		direction /= length;

		Prism::PhysicsInterface::GetInstance()->RayCast(myPreviousPosition, direction, length, myRaycastHandler
			, myEntity.GetComponent<PhysicsComponent>());
	}
	myPreviousPosition = myEntity.GetOrientation().GetPos();
}

void PlayerComponent::EvaluateDeath()
{
	if (myShouldDie == true)
	{
		myShouldDie = false;
		PostMaster::GetInstance()->SendMessage(ScrapMessage(eScrapPart::BODY, myEntity.GetOrientation().GetPos()
			, { 0.f, 0.f }, myEntity.GetComponent<InputComponent>()->GetPlayerID()));
		if (myEntity.GetComponent<PlayerGraphicsComponent>()->GetLegsActive() == true)
		{
			PostMaster::GetInstance()->SendMessage(ScrapMessage(eScrapPart::LEGS, myEntity.GetOrientation().GetPos()
				, { 0.f, 0.f }, myEntity.GetComponent<InputComponent>()->GetPlayerID()));
		}
		if (myEntity.GetComponent<PlayerGraphicsComponent>()->GetHeadActive() == true)
		{
			PostMaster::GetInstance()->SendMessage(ScrapMessage(eScrapPart::HEAD, myEntity.GetOrientation().GetPos()
				, { 0.f, 0.f }, myEntity.GetComponent<InputComponent>()->GetPlayerID()));
		}
		PostMaster::GetInstance()->SendMessage(OnDeathMessage(myEntity.GetComponent<InputComponent>()->GetPlayerID()));
		myEntity.Reset();
		myEntity.SendNote(DeathNote());
	}
}

void PlayerComponent::HandleCollision(Entity* aOther)
{
	if (aOther->GetType() == eEntityType::BOUNCER || aOther->GetType() == eEntityType::SCRAP)
	{
		return;
	}

	CU::Vector2<float> velocity = myEntity.GetComponent<MovementComponent>()->GetVelocity();
	if (CU::Length2(velocity) > myData.myDeathSpeed * myData.myDeathSpeed)
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

void PlayerComponent::HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection
	, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
{
	if (aComponent != nullptr)
	{
		const Entity& other(aComponent->GetEntity());
		if (other.GetType() == eEntityType::GOAL_POINT)
		{
			bool CanDanielMakeUsWinLevelHere = true; //?
		}
		else if (other.GetType() != eEntityType::SPAWN_POINT
			&& other.GetType() != eEntityType::PLAYER
			&& other.GetType() != eEntityType::SCRAP)
		{
			myEntity.SendNote<ShouldDieNote>(ShouldDieNote());
		}
	}
}