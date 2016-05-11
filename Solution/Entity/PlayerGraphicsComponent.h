#pragma once
#include <AnimationJoint.h>
#include "Component.h"
#include <Matrix.h>
#include "PlayerBody.h"
#include <Subscriber.h>
struct PlayerGraphicsComponentData;

namespace Prism
{
	class Instance;
	class Scene;
}

class PlayerGraphicsComponent : public Component, public Subscriber
{
public:
	PlayerGraphicsComponent(Entity& aEntity, const PlayerGraphicsComponentData& aData
		, const CU::Matrix44<float>& aEntityOrientation, Prism::Scene* aScene, int aPlayerID);
	~PlayerGraphicsComponent();

	void Init() override;
	void Reset() override;
	void Activate();

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;
	void ReceiveNote(const SpawnNote& aMessage) override;
	void ReceiveNote(const DeathNote& aMessage) override;

private:
	const PlayerGraphicsComponentData& myData;
	const CU::Matrix44<float>& myEntityOrientation;

	int myPlayerID;

	Prism::Scene* myScene;
	Prism::Instance* myArrow;

	CU::Matrix44<float> myArrowOrientation;

	BodyPart myBody;
	BodyPart myLeftLeg;
	BodyPart myRightLeg;
	BodyPart myHead;

	BodyAnimation myIdleAnimation;
	BodyAnimation myWalkAnimation;
	BodyAnimation myFlyAnimation;

	BodyAnimation* myCurrentAnimation;
};

inline eComponentType PlayerGraphicsComponent::GetTypeStatic()
{
	return eComponentType::PLAYER_GRAPHICS;
}

inline eComponentType PlayerGraphicsComponent::GetType()
{
	return GetTypeStatic();
}