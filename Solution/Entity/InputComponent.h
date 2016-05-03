#pragma once
#include "Component.h"

struct InputComponentData;

namespace CU
{
	class ControllerInput;
}

class InputComponent : public Component
{
public:
	InputComponent(Entity& aEntity, const InputComponentData& aInputData, CU::Matrix44f& anOrientation);
	~InputComponent();

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
private:
	const InputComponentData& myComponentData;
	CU::ControllerInput* myController;

	CU::Matrix44f& myOrientation;
};

inline eComponentType InputComponent::GetTypeStatic()
{
	return eComponentType::GRAPHICS;
}

inline eComponentType InputComponent::GetType()
{
	return GetTypeStatic();
}