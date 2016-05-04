#pragma once
#include <AnimationJoint.h>
#include "Component.h"
#include <Matrix.h>
#include "PlayerBody.h"

struct PlayerGraphicsComponentData;

namespace Prism
{
	class Instance;
	class Scene;
}

class PlayerGraphicsComponent : public Component
{
public:
	PlayerGraphicsComponent(Entity& aEntity, const PlayerGraphicsComponentData& aData
		, const CU::Matrix44<float>& aEntityOrientation, Prism::Scene* aScene);
	~PlayerGraphicsComponent();

	void Init() override;

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	const PlayerGraphicsComponentData& myData;
	const CU::Matrix44<float>& myEntityOrientation;

	Prism::Scene* myScene;

	BodyPart myBody;
	BodyPart myLeftLeg;
	BodyPart myRightLeg;
	BodyPart myHead;

	BodyJoints myIdleJoints;

	BodyJoints* myCurrentAnimationJoints;
	Prism::Instance* myCurrentAnimation;
	BodyAnimation myIdleJoints;
};

inline eComponentType PlayerGraphicsComponent::GetTypeStatic()
{
	return eComponentType::PLAYER_GRAPHICS;
}

inline eComponentType PlayerGraphicsComponent::GetType()
{
	return GetTypeStatic();
}
