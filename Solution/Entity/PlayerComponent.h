#pragma once
#include "Component.h"

class PlayerComponent : public Component
{
public:
	PlayerComponent(Entity& anEntity);
	~PlayerComponent();

	void Update(float aDeltaTime) override;
	void EvaluateDeath();
	void HandleCollision(Entity* aOther);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void ReceiveNote(const ShouldDieNote& aMessage) override;
	void ReceiveNote(const SpawnNote& aMessage) override;

private:
	bool myShouldDie;
};

inline eComponentType PlayerComponent::GetTypeStatic()
{
	return eComponentType::PLAYER;
}

inline eComponentType PlayerComponent::GetType()
{
	return GetTypeStatic();
}