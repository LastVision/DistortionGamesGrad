#pragma once
#include "Component.h"

namespace Prism
{
	class Scene;
}

class SteamComponent : public Component
{
public:
	SteamComponent(Entity& anEntity);
	~SteamComponent();

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void InitSteam(Prism::Scene* aScene, const CU::Vector3<float>& aRotation, const std::string& aSteamSubType, float aSteamInterval, float aSteamTime, float aDelayBeforeSteam);

	float GetForce() const;
	void SetForce(float aForce);

private:

	Entity* mySteam;

	float myDelayBeforeSteam;
	float mySteamInterval;
	float mySteamTime;

	float myCurrentSteamInterval;
	float myCurrentSteamTime;

	float myTimeBeforeSmoke;
	float myTimeBeforeVentOpens;

	bool myIsConstant;

	bool myIsSmoking;
	bool myIsOpen;
};

inline eComponentType SteamComponent::GetTypeStatic()
{
	return eComponentType::STEAM;
}

inline eComponentType SteamComponent::GetType()
{
	return GetTypeStatic();
}