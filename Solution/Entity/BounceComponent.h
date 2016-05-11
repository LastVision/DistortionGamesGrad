#pragma once
#include "Component.h"

class BounceComponent : public Component
{
public:
	BounceComponent(Entity& anEntity);
	~BounceComponent();

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	float GetForce() const;
	void SetForce(float aForce);

private:

	float myForce;
};

inline eComponentType BounceComponent::GetTypeStatic()
{
	return eComponentType::BOUNCE;
}

inline eComponentType BounceComponent::GetType()
{
	return GetTypeStatic();
}

inline float BounceComponent::GetForce() const
{
	return myForce;
}

inline void BounceComponent::SetForce(float aForce)
{
	myForce = aForce;
}