#pragma once
#include "Component.h"
#include <Matrix.h>

struct PlayerGraphicsComponentData;

namespace Prism
{
	class Instance;
}

class PlayerGraphicsComponent : public Component
{
public:
	PlayerGraphicsComponent(Entity& aEntity, const PlayerGraphicsComponentData& aData
		, const CU::Matrix44<float>& aEntityOrientation);
	~PlayerGraphicsComponent();

	void Init() override;

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	const PlayerGraphicsComponentData& myData;
	const CU::Matrix44<float>& myEntityOrientation;

	Prism::Instance* myBody;
	Prism::Instance* myBindPose;

	CU::Matrix44<float> myBodyMatrix;
};

inline eComponentType PlayerGraphicsComponent::GetTypeStatic()
{
	return eComponentType::PLAYER_GRAPHICS;
}

inline eComponentType PlayerGraphicsComponent::GetType()
{
	return GetTypeStatic();
}
