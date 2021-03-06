#pragma once
#include "Component.h"
#include <Subscriber.h>

class SawBladeComponent : public Component, public Subscriber
{
public:
	SawBladeComponent(Entity& anEntity);
	~SawBladeComponent();

	void Update(float aDeltaTime) override;

	void Init() override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void SetPatrol(const CU::GrowingArray<CU::Vector3<float>>& somePositions, float aSpeed, float aDelayBeforePatrol = 0.f);
	CU::Vector3f GetParticlePos() const;

	void ReceiveMessage(const SoundMessage& aMessage) override;
private:

	CU::GrowingArray<CU::Vector3<float>> myPositions;

	CU::Vector3<float> myStartPosition;

	float myPatrolSpeed;
	float myDelayBeforePatrol;
	float myTotalCurrentLength;

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