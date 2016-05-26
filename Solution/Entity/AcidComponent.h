#pragma once
#include "Component.h"

class AcidComponent : public Component
{
public:
	AcidComponent(Entity& anEntity);
	~AcidComponent();

	void Update(float aDeltaTime) override;

	void InitAcid(int anAmount, float aAcidIntervalMax, float aAcidIntervalMin
		, CU::Vector3<float> aMaxVelocity, CU::Vector3<float> aMinVelocity, Prism::Scene* aScene);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:

	CU::GrowingArray<Entity*> myAcidDrops;

	CU::Vector3<float> mySpawnPosition;
	CU::Vector3<float> mySpawnVelocityMin;
	CU::Vector3<float> mySpawnVelocityMax;

	float myAcidTimer;
	float myAcidIntervalMin;
	float myAcidIntervalMax;

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