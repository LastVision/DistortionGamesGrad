#pragma once

class Entity;

namespace Prism
{
	class Scene;
}

class ScrapManager
{
public:
	static void Create(Prism::Scene* aScene);
	static void Destroy();
	static ScrapManager* GetInstance();

	void Update(float aDeltaTime);
	void SpawnScrap(eScrapPart aPart, const CU::Vector3<float>& aPosition, const CU::Vector2<float>& aVelocity);


private:
	ScrapManager(Prism::Scene* aScene);
	~ScrapManager();

	static ScrapManager* myInstance;

	void CreateHeads();
	struct Head
	{
		Head::Head() : myHead(nullptr), myTimer(0.f), myMaxTime(10.f){}
		Entity* myHead;
		float myTimer;
		float myMaxTime;
	};

	CU::GrowingArray<Head> myHeads;
	CU::GrowingArray<Head> myLiveHeads;
	int myHeadIndex;

	Prism::Scene* myScene;
};

