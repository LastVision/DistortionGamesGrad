#pragma once
#include "Component.h"

struct MovementComponentData;

class MovementComponent : public Component
{
public:
	MovementComponent(Entity& aEntity, const MovementComponentData& aInputData, CU::Matrix44f& anOrientation);
	~MovementComponent();

	void Update(float aDeltaTime) override;
	void ReceiveNote(const ContactNote& aNote) override;

	void Impulse();
	void SetUpTarget(const CU::Vector2<float>& anUp);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	const MovementComponentData& myData;

	void Drag(float aDeltaTime);
	void Rotate(float aDeltaTime);
	void Translate();

	CU::Vector2<float> myVelocity;

	CU::Vector2<float> myUpTarget;

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

inline void MovementComponent::SetUpTarget(const CU::Vector2<float>& anUp)
{
	myUpTarget = anUp;
}