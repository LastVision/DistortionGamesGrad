#pragma once
#include "Component.h"

struct MovementComponentData;

namespace CU
{
	class ControllerInput;
}

class MovementComponent : public Component
{
public:
	MovementComponent(Entity& aEntity, const MovementComponentData& aInputData, CU::Matrix44f& anOrientation);
	~MovementComponent();

	void Update(float aDeltaTime) override;

	void Impulse();

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
private:
	const MovementComponentData& myComponentData;
	CU::ControllerInput* myController;

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