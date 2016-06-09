#pragma once
#include <AnimationJoint.h>
class Entity;

namespace Prism
{
	class Scene;
}

class Hat;

class ScrapManager : public Subscriber
{
public:
	ScrapManager(Prism::Scene* aScene, int aPlayerID);
	~ScrapManager();

	void Update(float aDeltaTime);
	void SpawnScrap(eScrapPart aPart, const CU::Vector3<float>& aPosition, const CU::Vector2<float>& aVelocity);

	void ReceiveMessage(const ScrapMessage& aMessage) override;

	void CreateHeads();

private:
	int myPlayerID;
	void CreateLegs();
	void CreateBodies();
	struct BodyPart
	{
		BodyPart::BodyPart() : myEntity(nullptr), myTimer(0.f), myMaxTime(10.f){}
		Entity* myEntity;
		float myTimer;
		float myMaxTime;
	};

	struct HeadPart
	{
		HeadPart::HeadPart() : myEntity(nullptr), myTimer(0.f), myMaxTime(10.f), myHat(nullptr){}
		Entity* myEntity;
		float myTimer;
		float myMaxTime;
		
		Hat* myHat;
		AnimationJoint myJoint;
	};

	CU::GrowingArray<HeadPart> myHeads;
	CU::GrowingArray<HeadPart*> myLiveHeads;
	int myHeadIndex;

	CU::GrowingArray<BodyPart> myLegs;
	CU::GrowingArray<BodyPart*> myLiveLegs;
	int myLegIndex;

	CU::GrowingArray<BodyPart> myBodies;
	CU::GrowingArray<BodyPart*> myLiveBodies;
	int myBodyIndex;

	void CreateGibs();
	struct GibPart
	{
		GibPart::GibPart() : mySpring(nullptr), myScrewNut(nullptr), myScrew(nullptr), myTimer(0.f), myMaxTime(10.f){}
		Entity* mySpring;
		Entity* myScrewNut;
		Entity* myScrew;
		float myTimer;
		float myMaxTime;
	};

	CU::GrowingArray<GibPart> myGibs;
	CU::GrowingArray<GibPart*> myLiveGibs;
	int myGibIndex;

	Prism::Scene* myScene;
};

