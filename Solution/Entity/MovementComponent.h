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
	void SetInSteam(bool aIsInSteam, float aForce = 1.f, float aSteamLength = 0.f, const CU::Vector2<float>& aDirection = { 0.f, 0.f }
		, const CU::Vector3<float>& anOrigion = { 0.f, 0.f, 0.f });
	void SetVelocity(const CU::Vector2<float>& aVelocity);
	void ReceiveNote(const DeathNote& aMessage) override;
	void ReceiveNote(const SpawnNote& aMessage) override;

	const CU::Vector2<float>& GetVelocity() const;
	const CU::Vector2<float>& GetAverageVelocity() const;
	void SetSpawnVelocity(const CU::Vector2<float>& aSpawnVelocity);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	bool IsInDashFly() const;
	bool GetShouldCollide() const;

private:
	const MovementComponentData& myData;

	eMovementType myCurrentMovement;

	CU::StaticArray<Movement*, eMovementType::_COUNT> myMovements;
	CU::Vector2<float> myVelocity;
	CU::Vector2<float> myAverageVelocity;
	CU::GrowingArray<CU::Vector2<float>> myVelocities;
	int myVelocityIndex;
	float myAverageVelocityTimer;

	float myDashCooldown;

	bool myIsInSteam;
	float mySteamLength;
	CU::Vector2<float> mySteamVelocity;
	CU::Vector3<float> mySteamOrigin;


	float myDeltaTime;

	CU::Vector2<float> mySpawnVelocity;
	float myCollisionTimer;
	bool myShouldCollide;
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

inline bool MovementComponent::IsInDashFly() const
{
	return myCurrentMovement == eMovementType::DASH_FLY;
}

inline bool MovementComponent::GetShouldCollide() const
{
	return myShouldCollide;
}

inline const CU::Vector2<float>& MovementComponent::GetAverageVelocity() const
{
	return myAverageVelocity;
}