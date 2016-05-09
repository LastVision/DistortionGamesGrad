#pragma once
#include "Component.h"
#include "ContactNote.h"
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

	void Reset() override;

	void Update(float aDeltaTime) override;
	void ReceiveNote(const ContactNote& aNote) override;

	void Impulse();
	void Impulse(const CU::Vector2<float>& aVelocity);
	void SetDirectionTarget(const CU::Vector2<float>& aDirection);
	void RightTriggerDown();
	void RightTriggerUp();

	void SetState(eMovementType aState);
	void SetInSteam(bool aIsInSteam, const CU::Vector2<float>& aVelocity = { 0.f, 0.f });
	void SetVelocity(const CU::Vector2<float>& aVelocity);

	void ReceiveMessage(const OnDeathMessage& aMessage) override;
	void ReceiveMessage(const PlayerActiveMessage& aMessage) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	const MovementComponentData& myData;

	eMovementType myCurrentMovement;

	CU::StaticArray<Movement*, eMovementType::_COUNT> myMovements;
	float myDashCooldown;

	bool myIsInSteam;
	CU::Vector2<float> mySteamVelocity;
};

inline eComponentType MovementComponent::GetTypeStatic()
{
	return eComponentType::MOVEMENT;
}

inline eComponentType MovementComponent::GetType()
{
	return GetTypeStatic();
}

inline void MovementComponent::SetInSteam(bool aIsInSteam, const CU::Vector2<float>& aVelocity)
{
	myIsInSteam = aIsInSteam;
	mySteamVelocity = aVelocity;
}
