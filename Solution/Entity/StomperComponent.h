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

	bool IsGoingDown() const;
	bool IsGoingUp() const;

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

inline bool StomperComponent::IsGoingDown() const
{
	return myStompDownTimer > 0.f && myHasReachedMax == false;
}

inline bool StomperComponent::IsGoingUp() const
{
	return myBeforeStompTimer > 0.f && myHasReachedMin == false;
}
