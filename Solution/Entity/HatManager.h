#pragma once

namespace Prism
{
	class ModelProxy;
}

class HatManager
{
public:
	static void Create();
	static void Destroy();
	static HatManager* GetInstance();

	void LoadHats();
	void UnlockHat(int aID);

	void SetHatOnPlayer(int aPlayerID, int aHatID);
	int GetHatIDOnPlayer(int aPlayerID) const;
	Prism::ModelProxy* GetHat(int aID);
	bool IsHatUnlocked(int aID) const;

	int GetAmountOfHats() const;

	bool IsAllHatsUnlocked() const;
	bool IsAllHatsLocked() const;

private:
	HatManager();
	~HatManager();
	static HatManager* myInstance;

	void Save();

	std::unordered_map<int, Prism::ModelProxy*> myHats;

	CU::GrowingArray<int> myPlayersCurrentHat;
	CU::GrowingArray<int> myHatsStatus;

};

