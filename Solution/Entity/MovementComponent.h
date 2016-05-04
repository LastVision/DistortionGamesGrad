#pragma once
#include "Component.h"
#include "ContactNote.h"

struct MovementComponentData;

class Movement;

class MovementComponent : public Component
{
public:
	MovementComponent(Entity& aEntity, const MovementComponentData& aData, CU::Matrix44f& anOrientation);
	~MovementComponent();

	void Reset() override;

	void Update(float aDeltaTime) override;
	void ReceiveNote(const ContactNote& aNote) override;

	void Impulse();
	void SetDirectionTarget(const CU::Vector2<float>& aDirection);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:

	enum eMovementType
	{
		FLY,
		WALK,
		DASH_AIM,
		DASH_FLY,
		_COUNT,
	};

	const MovementComponentData& myData;

	eMovementType myCurrentMovement;

	CU::StaticArray<Movement*, eMovementType::_COUNT> myMovements;

};

inline eComponentType MovementComponent::GetTypeStatic()
{
	return eComponentType::MOVEMENT;
}

inline eComponentType MovementComponent::GetType()
{
	return GetTypeStatic();
}

