#pragma once
#include "Component.h"

namespace CU
{
	class ControllerInput;
}

class MovementComponent;

struct InputComponentData;


class InputComponent : public Component
{
public:
	InputComponent(Entity& aEntity, const InputComponentData& aInputData);
	~InputComponent();

	void Init() override;

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
private:
	const InputComponentData& myComponentData;
	CU::ControllerInput* myController;

	MovementComponent* myMovement;

};

inline eComponentType InputComponent::GetTypeStatic()
{
	return eComponentType::GRAPHICS;
}

inline eComponentType InputComponent::GetType()
{
	return GetTypeStatic();
}