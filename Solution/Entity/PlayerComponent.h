#pragma once
#include "Component.h"

class PlayerComponent : public Component
{
public:
	PlayerComponent(Entity& anEntity);
	~PlayerComponent();

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void ReceiveNote(const DeathNote& aMessage) override;


private:
};

inline eComponentType PlayerComponent::GetTypeStatic()
{
	return eComponentType::PLAYER;
}

inline eComponentType PlayerComponent::GetType()
{
	return GetTypeStatic();
}