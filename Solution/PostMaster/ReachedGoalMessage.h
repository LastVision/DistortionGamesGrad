#pragma once
#include "Message.h"

class Entity;
struct ReachedGoalMessage : public Message
{
	ReachedGoalMessage(const Entity* aGoalEntity, const Entity* aPlayer);
	const Entity* myGoalEntity;
	const Entity* myPlayer;
};

inline ReachedGoalMessage::ReachedGoalMessage(const Entity* aGoalEntity, const Entity* aPlayer)
	: Message(eMessageType::REACHED_GOAL)
	, myGoalEntity (aGoalEntity)
	, myPlayer(aPlayer)
{	
}