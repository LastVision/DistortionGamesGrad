#pragma once
#include "Component.h"

class StomperComponent : public Component
{
public:
	StomperComponent(Entity& anEntity, Prism::Scene* aScene, const CU::Vector3<float>& aRotation);
	~StomperComponent();

	void Update(float aDeltaTime) override;

	void InitStomper(float aTimeBeforeStomp, float aTimeStomperIsDown, float aStomperSpeedOut
		, float aStomperSpeedIn, float aStomperDistance, float aDelayBeforeStart);

	static eComponentType GetTypeStatic();

	eComponentType GetType() override;

private:
	Entity* myStomper;

	CU::Vector3<float> myStomperMaxPosition;

	float myTimeBeforeStomp;
	float myTimeStomperDown;
	float myStompSpeedOut;
	float myStompSpeedIn;
	float myDistance;
	float myDelayBeforeStomp;

	float myBeforeStompTimer;
	float myStompDownTimer;
	float myToStompLength;

	bool myHasReachedMax;
	bool myHasReachedMin;
};

inline eComponentType StomperComponent::GetTypeStatic()
{
	return eComponentType::STOMPER;
}

inline eComponentType StomperComponent::GetType()
{
	return GetTypeStatic();
}