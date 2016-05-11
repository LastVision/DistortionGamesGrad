#pragma once
#include "Component.h"
#include "AnimationJoint.h"

namespace Prism
{
	class Animation;
	class Instance;
	struct MeshData;
	class Camera;
	class Texture;
	class Terrain;
	class Scene;
};

struct AnimationComponentData;

class AnimationComponent : public Component
{
public:
	AnimationComponent(Entity& aEntity, const AnimationComponentData& aComponentData, Prism::Scene* aScene);
	~AnimationComponent();

	void Reset() override;

	void Update(float aDeltaTime);
	Prism::Instance* GetInstance();
	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	struct AnimationData
	{
		AnimationData() 
			: myFile("Not loaded")
			, myShouldLoop(false)
			, myResetTimeOnRestart(false)
			, myElapsedTime(0)
		{}
		std::string myFile;
		bool myShouldLoop;
		bool myResetTimeOnRestart;
		float myElapsedTime;
	};



	Prism::Instance* myInstance;
	float myCullingRadius;
	AnimationData myAnimation;

	const AnimationComponentData& myComponentData;
};

inline Prism::Instance* AnimationComponent::GetInstance()
{
	return myInstance;
}

inline eComponentType AnimationComponent::GetTypeStatic()
{
	return eComponentType::ANIMATION;
}

inline eComponentType AnimationComponent::GetType()
{
	return GetTypeStatic();
}