#pragma once
#include "Component.h"

class SteamComponent : public Component
{
public:
	SteamComponent(Entity& anEntity);
	~SteamComponent();

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:

	Entity* mySteam;
};

inline eComponentType SteamComponent::GetTypeStatic()
{
	return eComponentType::STEAM;
}

inline eComponentType SteamComponent::GetType()
{
	return GetTypeStatic();
}