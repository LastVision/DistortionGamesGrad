#pragma once
#include "Component.h"

class SawBladeComponent : public Component
{
public:
	SawBladeComponent(Entity& anEntity);
	~SawBladeComponent();

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void SetPatrol(const CU::GrowingArray<CU::Vector3<float>>& somePositions);
private:

	CU::GrowingArray<CU::Vector3<float>> myPositions;
	bool myIsPatrolling;

};

inline eComponentType SawBladeComponent::GetTypeStatic()
{
	return eComponentType::SAW_BLADE;
}

inline eComponentType SawBladeComponent::GetType()
{
	return GetTypeStatic();
}

inline void SawBladeComponent::SetPatrol(const CU::GrowingArray<CU::Vector3<float>>& somePositions)
{
	myIsPatrolling = true;
	myPositions = somePositions;
}