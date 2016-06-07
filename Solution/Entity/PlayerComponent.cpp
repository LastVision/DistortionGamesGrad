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
#include <ReachedGoalMessage.h>
#include "ReachedGoalNote.h"
#include "Score.h"
#include "ScoreComponent.h"
#include "ShouldDieNote.h"
#include <ScrapMessage.h>
#include <EmitterMessage.h>

PlayerComponent::PlayerComponent(Entity& anEntity, const PlayerComponentData& aData)
	: Component(anEntity)
	, myData(aData)
	, myShouldDie(false)
	, myIsAlive(true)
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
	myPreviousPositions[0] = myEntity.GetOrientation().GetPos();
	myPreviousPositions[1] = myEntity.GetOrientation().GetPos();
}

void PlayerComponent::Update(float)
{
	CU::Vector3<float> position(myEntity.GetOrientation().GetPos());

	if ((position.x < 0.f || position.x > 70.f || position.y < 0.f || position.y > 38.f)
		&& myEntity.GetComponent<ScoreComponent>()->GetScore()->myReachedGoal == false)
	{
		myEntity.SendNote(ShouldDieNote());
	}

	if (myEntity.GetComponent<MovementComponent>()->GetShouldCollide() == true)
	{
		CU::Vector3<float> direction(myPreviousPositions[0] - myPreviousPositions[1]);
		CU::Vector3<float> position(myPreviousPositions[1]);

		if (direction != CU::Vector3<float>())
		{
			float length = CU::Length(direction);

			direction /= length;

			Prism::PhysicsInterface::GetInstance()->RayCast(position + CU::Vector3<float>(0.5f, 0, 0), direction, length, myRaycastHandler
				, myEntity.GetComponent<PhysicsComponent>());
		}
	}
	myPreviousPositions[1] = myPreviousPositions[0];
	myPreviousPositions[0] = myEntity.GetOrientation().GetPos();
}

void PlayerComponent::EvaluateDeath()
{
	if (myShouldDie == true)
	{
		myShouldDie = false;
		myIsAlive = false;
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

	if (myEntity.GetComponent<MovementComponent>()->GetShouldCollide() == false)
	{
		return;
	}

	CU::Vector2<float> velocity = myEntity.GetComponent<MovementComponent>()->GetVelocity();
	if (CU::Length2(velocity) > myData.myDeathSpeed * myData.myDeathSpeed)
	{
		myEntity.SendNote(ShouldDieNote());
		if (aOther->GetType() == eEntityType::PROP)
		{
			CU::Vector3f dir = aOther->GetOrientation().GetPos() - myEntity.GetOrientation().GetPos();
			CU::Normalize(dir);
			PostMaster::GetInstance()->SendMessage(EmitterMessage("Prop_Death", myEntity.GetOrientation().GetPos(), true, dir, true));
		}
	}
}

void PlayerComponent::ReceiveNote(const ShouldDieNote&)
{
	if (myIsAlive == true)
	{
		myShouldDie = true;
	}
}

void PlayerComponent::ReceiveNote(const SpawnNote&)
{
	PostMaster::GetInstance()->SendMessage(PlayerActiveMessage(true, myEntity.GetComponent<InputComponent>()->GetPlayerID()));
	myIsAlive = true;
}

void PlayerComponent::ReceiveNote(const ReachedGoalNote& aMessage)
{
}

void PlayerComponent::HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection
	, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
{
	if (aComponent != nullptr)
	{
		const Entity& other(aComponent->GetEntity());
		if (other.GetType() == eEntityType::GOAL_POINT)
		{
			myEntity.SendNote(ReachedGoalNote(&aComponent->GetEntity()));
		}
		else if (other.GetType() != eEntityType::SPAWN_POINT
			&& other.GetType() != eEntityType::PLAYER
			&& other.GetType() != eEntityType::SCRAP)
		{
			myEntity.SendNote<ShouldDieNote>(ShouldDieNote());

			if (other.GetType() == eEntityType::PROP)
			{
				PostMaster::GetInstance()->SendMessage(EmitterMessage("Prop_Death", aHitPosition, true, aHitNormal, true));
			}

		}
	}
}

float PlayerComponent::GetDeathSpeed() const
{
	return myData.myDeathSpeed;
}