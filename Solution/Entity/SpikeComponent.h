#pragma once
#include "Component.h"

class SpikeComponent : public Component
{
public:
	SpikeComponent(Entity& anEntity);
	~SpikeComponent();

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:



};

inline eComponentType SpikeComponent::GetTypeStatic()
{
	return eComponentType::SPIKE;
}

inline eComponentType SpikeComponent::GetType()
{
	return GetTypeStatic();
}