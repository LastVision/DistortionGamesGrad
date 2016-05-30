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

	Prism::ModelProxy* GetHat(int aID);
private:
	HatManager();
	~HatManager();
	static HatManager* myInstance;

	std::unordered_map<int, Prism::ModelProxy*> myHats;
};

