#pragma once
#include "Message.h"

class Entity;
struct ReachedGoalMessage : public Message
{
	ReachedGoalMessage(Entity* aGoalEntity);
	Entity* myGoalEntity;
};

inline ReachedGoalMessage::ReachedGoalMessage(Entity* aGoalEntity)
	: Message(eMessageType::REACHED_GOAL)
{
	myGoalEntity = aGoalEntity;
}