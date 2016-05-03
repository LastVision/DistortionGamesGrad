#pragma once
#include <AnimationJoint.h>
#include "Component.h"
#include <Matrix.h>

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
	struct BodyPart
	{
		~BodyPart();

		void UpdateOrientation(const CU::Matrix44<float>& aEntityOrientation, AnimationJoint& aJoint);

		Prism::Instance* myInstance;
		CU::Matrix44<float> myOrientation;
	};

	struct BodyJoints
	{
		AnimationJoint myHead;
		AnimationJoint myBody;
		AnimationJoint myLeftLeg;
		AnimationJoint myRightLeg;
		AnimationJoint myJetPack;
	};


	const PlayerGraphicsComponentData& myData;
	const CU::Matrix44<float>& myEntityOrientation;

	Prism::Instance* myBindPose;
	Prism::Scene* myScene;

	BodyPart myBody;
	BodyPart myLeftLeg;
	BodyPart myRightLeg;
	BodyPart myHead;

	BodyJoints myIdleJoints;
};

inline eComponentType PlayerGraphicsComponent::GetTypeStatic()
{
	return eComponentType::PLAYER_GRAPHICS;
}

inline eComponentType PlayerGraphicsComponent::GetType()
{
	return GetTypeStatic();
}
