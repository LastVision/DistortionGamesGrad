#pragma once

class ScrapManager
{
public:
	ScrapManager();
	~ScrapManager();

	void Update(float aDeltaTime);



private:
	CU::GrowingArray<Entity*> myHeads;
	CU::GrowingArray<Entity*> myLiveHeads;
};

