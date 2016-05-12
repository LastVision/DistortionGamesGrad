#pragma once
#include "Component.h"

class SawBladeComponent : public Component
{
public:
	SawBladeComponent(Entity& anEntity);
	~SawBladeComponent();

	void Update(float aDeltaTime) override;

	void Init() override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void SetPatrol(const CU::GrowingArray<CU::Vector3<float>>& somePositions, float aSpeed, float aDelayBeforePatrol = 0.f);
private:

	CU::GrowingArray<CU::Vector3<float>> myPositions;
	float myPatrolSpeed;
	float myDelayBeforePatrol;
	bool myIsLoopingForward;
	int myCurrentIndex;
};

inline eComponentType SawBladeComponent::GetTypeStatic()
{
	return eComponentType::SAW_BLADE;
}

inline eComponentType SawBladeComponent::GetType()
{
	return GetTypeStatic();
}

inline void SawBladeComponent::SetPatrol(const CU::GrowingArray<CU::Vector3<float>>& somePositions, float aSpeed, float aDelayBeforePatrol)
{
	myPositions = somePositions;
	myPatrolSpeed = aSpeed;
	myDelayBeforePatrol = aDelayBeforePatrol;
}