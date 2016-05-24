#pragma once
#include "Component.h"
#include "ContactNote.h"
#include <GameEnum.h>
#include "Subscriber.h"

struct MovementComponentData;

class Movement;

class MovementComponent : public Component, public Subscriber
{
public:
	enum eMovementType
	{
		FLY,
		WALK,
		DASH_AIM,
		DASH_FLY,
		_COUNT,
	};

	MovementComponent(Entity& aEntity, const MovementComponentData& aData, CU::Matrix44f& anOrientation, Prism::Scene* aScene);
	~MovementComponent();

	void Init() override;
	void Reset() override;

	void Update(float aDeltaTime) override;
	void Render();
	void ReceiveNote(const ContactNote& aNote) override;

	void Impulse();
	void Impulse(const CU::Vector2<float>& aVelocity);
	void SetDirectionTarget(const CU::Vector2<float>& aDirection);
	void RightTriggerDown();
	void RightTriggerUp();

	void SetState(eMovementType aState, const CU::Vector2<float>& aVelocity);
	void SetInSteam(bool aIsInSteam, const CU::Vector2<float>& aVelocity = { 0.f, 0.f });
	void SetVelocity(const CU::Vector2<float>& aVelocity);
	void ReceiveNote(const DeathNote& aMessage) override;
	void ReceiveNote(const SpawnNote& aMessage) override;

	const CU::Vector2<float>& GetVelocity() const;
	void SetSpawnVelocity(const CU::Vector2<float>& aSpawnVelocity);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	const MovementComponentData& myData;

	eMovementType myCurrentMovement;

	CU::StaticArray<Movement*, eMovementType::_COUNT> myMovements;
	float myDashCooldown;

	bool myIsInSteam;
	CU::Vector2<float> mySteamVelocity;

	float myDeltaTime;

	CU::Vector2<float> mySpawnVelocity;
	float myCollisionTimer;
};

inline void MovementComponent::SetSpawnVelocity(const CU::Vector2<float>& aSpawnVelocity)
{
	mySpawnVelocity = aSpawnVelocity;
}

inline eComponentType MovementComponent::GetTypeStatic()
{
	return eComponentType::MOVEMENT;
}

inline eComponentType MovementComponent::GetType()
{
	return GetTypeStatic();
}