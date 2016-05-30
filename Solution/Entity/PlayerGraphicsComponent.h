#pragma once
#include <AnimationJoint.h>
#include "Component.h"
#include <GameEnum.h>
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
	void ReceiveNote(const LoseBodyPartNote& aMessage) override;
	void ReceiveNote(const SpawnNote& aMessage) override;
	void ReceiveNote(const DeathNote& aMessage) override;
	void ReceiveNote(const CharacterAnimationNote& aMessage) override;

	bool GetLegsActive() const;
	bool GetHeadActive() const;
	BodyAnimation* GetCurrentAnimation();
private:
	const PlayerGraphicsComponentData& myData;
	const CU::Matrix44<float>& myEntityOrientation;

	int myPlayerID;

	Prism::Scene* myScene;
	Prism::Instance* myArrow;
	bool myShowArrow;
	float myLerping;

	CU::Matrix44<float> myArrowOrientation;

	BodyPart myLeftLeg;
	BodyPart myRightLeg;
	Head myHead;

	BodyAnimation myIdleAnimation;
	BodyAnimation myWalkAnimation;
	BodyAnimation myFlyAnimation;
	BodyAnimation myDashAimAnimation;
	BodyAnimation myDashFlyAnimation;

	BodyAnimation* myCurrentAnimation;
	eCharacterAnimationType myCurrentAnimationType; // debug syfte

	eCharacterAnimationType myPreviousAnimation;
};

inline eComponentType PlayerGraphicsComponent::GetTypeStatic()
{
	return eComponentType::PLAYER_GRAPHICS;
}

inline eComponentType PlayerGraphicsComponent::GetType()
{
	return GetTypeStatic();
}

inline bool PlayerGraphicsComponent::GetLegsActive() const
{
	return myRightLeg.GetActive();
}

inline bool PlayerGraphicsComponent::GetHeadActive() const
{
	return myHead.GetActive();
}