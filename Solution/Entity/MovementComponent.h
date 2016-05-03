#pragma once
#include "Component.h"

struct MovementComponentData;

class MovementComponent : public Component
{
public:
	MovementComponent(Entity& aEntity, const MovementComponentData& aInputData, CU::Matrix44f& anOrientation);
	~MovementComponent();

	void Update(float aDeltaTime) override;

	void Impulse();
	void Rotate(float aValue);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	const MovementComponentData& myData;

	void Drag(float aDeltaTime);

	CU::Vector2<float> myVelocity;

	CU::Matrix44f& myOrientation;
};

inline eComponentType MovementComponent::GetTypeStatic()
{
	return eComponentType::MOVEMENT;
}

inline eComponentType MovementComponent::GetType()
{
	return GetTypeStatic();
}