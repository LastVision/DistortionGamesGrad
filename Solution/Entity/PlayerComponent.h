#pragma once
#include "Component.h"
#include <functional>

struct PlayerComponentData;

class PlayerComponent : public Component
{
public:
	PlayerComponent(Entity& anEntity, const PlayerComponentData& aData);
	~PlayerComponent();

	void Reset() override;

	void Update(float aDeltaTime) override;
	void EvaluateDeath();
	void HandleCollision(Entity* aOther);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	const PlayerComponentData* GetData() const;

	void ReceiveNote(const ShouldDieNote& aMessage) override;
	void ReceiveNote(const SpawnNote& aMessage) override;
	void ReceiveNote(const ReachedGoalNote& aMessage) override;


	void HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection
		, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal);

	float GetDeathSpeed() const;

private:
	const PlayerComponentData& myData;
	bool myShouldDie;
	bool myIsAlive;
	std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&
		, const CU::Vector3<float>&)> myRaycastHandler;
	CU::Vector3<float> myPreviousPositions[2];
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