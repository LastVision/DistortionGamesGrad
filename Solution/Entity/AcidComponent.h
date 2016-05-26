#pragma once
#include "Component.h"

class AcidComponent : public Component
{
public:
	AcidComponent(Entity& anEntity, Prism::Scene* aScene);
	~AcidComponent();

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:

	CU::GrowingArray<Entity*> myAcidDrops;

	CU::Vector3<float> mySpawnPosition;

	float myAcidTimer;
	float myAcidInterval;

	int myAcidIndex;
};

inline eComponentType AcidComponent::GetTypeStatic()
{
	return eComponentType::ACID;
}

inline eComponentType AcidComponent::GetType()
{
	return GetTypeStatic();
}