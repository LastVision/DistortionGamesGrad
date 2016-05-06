#pragma once
#include "Component.h"

namespace Prism
{
	class Scene;
}

class SteamComponent : public Component
{
public:
	SteamComponent(Entity& anEntity, Prism::Scene* aScene, const CU::Vector3<float>& aRotation);
	~SteamComponent();

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void SetSteamVariables(float aSteamInterval, float aSteamTime, float aDelayBeforeSteam);

private:

	Entity* mySteam;

	float myDelayBeforeSteam;
	float mySteamInterval;
	float mySteamTime;

	float myCurrentSteamInterval;
	float myCurrentSteamTime;

	bool myIsConstant;
};

inline eComponentType SteamComponent::GetTypeStatic()
{
	return eComponentType::STEAM;
}

inline eComponentType SteamComponent::GetType()
{
	return GetTypeStatic();
}