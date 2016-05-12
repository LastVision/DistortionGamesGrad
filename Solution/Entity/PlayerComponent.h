#pragma once
#include "Component.h"

struct PlayerComponentData;

class PlayerComponent : public Component
{
public:
	PlayerComponent(Entity& anEntity, const PlayerComponentData& aData);
	~PlayerComponent();

	void Update(float aDeltaTime) override;
	void EvaluateDeath();
	void HandleCollision(Entity* aOther);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	const PlayerComponentData* GetData() const;

	void ReceiveNote(const ShouldDieNote& aMessage) override;
	void ReceiveNote(const SpawnNote& aMessage) override;

private:
	const PlayerComponentData& myData;
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

inline const PlayerComponentData* PlayerComponent::GetData() const
{
	return &myData;
}