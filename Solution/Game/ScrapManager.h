#pragma once

class Entity;

namespace Prism
{
	class Scene;
}

class ScrapManager : public Subscriber
{
public:
	static void Create(Prism::Scene* aScene);
	static void Destroy();
	static ScrapManager* GetInstance();

	void Update(float aDeltaTime);
	void SpawnScrap(eScrapPart aPart, const CU::Vector3<float>& aPosition, const CU::Vector2<float>& aVelocity);

	void ReceiveMessage(const ScrapMessage& aMessage) override;


private:
	ScrapManager(Prism::Scene* aScene);
	~ScrapManager();

	static ScrapManager* myInstance;

	void CreateHeads();
	void CreateLegs();
	void CreateBodies();
	struct BodyPart
	{
		BodyPart::BodyPart() : myEntity(nullptr), myTimer(0.f), myMaxTime(10.f){}
		Entity* myEntity;
		float myTimer;
		float myMaxTime;
	};

	CU::GrowingArray<BodyPart> myHeads;
	CU::GrowingArray<BodyPart> myLiveHeads;
	int myHeadIndex;

	CU::GrowingArray<BodyPart> myLegs;
	CU::GrowingArray<BodyPart> myLiveLegs;
	int myLegIndex;

	CU::GrowingArray<BodyPart> myBodies;
	CU::GrowingArray<BodyPart> myLiveBodies;
	int myBodyIndex;

	Prism::Scene* myScene;
};

