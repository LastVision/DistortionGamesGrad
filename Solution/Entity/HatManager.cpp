#include "stdafx.h"
#include "HatManager.h"
#include <ModelLoader.h>

HatManager* HatManager::myInstance = nullptr;

void HatManager::Create()
{
	DL_ASSERT_EXP(myInstance == nullptr, "HatManager already created.");
	myInstance = new HatManager();
}

void HatManager::Destroy()
{
	SAFE_DELETE(myInstance);
}

HatManager* HatManager::GetInstance()
{
	DL_ASSERT_EXP(myInstance != nullptr, "HatManager not created.");
	return myInstance;
}

void HatManager::LoadHats()
{
	XMLReader hatDocument;
	hatDocument.OpenDocument("Data/Setting/HatManager.xml");

	tinyxml2::XMLElement* rootElement = hatDocument.ForceFindFirstChild("root");
	//tinyxml2::XMLElement* entityElement = entityDocument.ForceFindFirstChild(rootElement, "Entity");
	std::string modelPath;
	int hatID;
	CU::Matrix44<float> tempMatrix;
	for (tinyxml2::XMLElement* hatElement = hatDocument.ForceFindFirstChild(rootElement, "Hat"); hatElement != nullptr;
		hatElement = hatDocument.FindNextElement(hatElement))
	{
		hatDocument.ForceReadAttribute(hatDocument.ForceFindFirstChild(hatElement, "Model"), "path", modelPath);
		Prism::ModelLoader::GetInstance()->AddHatPath(modelPath);
		hatDocument.ForceReadAttribute(hatDocument.ForceFindFirstChild(hatElement, "ID"), "value", hatID);

		myHats[hatID] = Prism::ModelLoader::GetInstance()->LoadModel(modelPath, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	}

	hatDocument.CloseDocument();

	std::string hatPath = "Data/UnlockedHats.bin";

	if (CU::FileExists(CU::GetMyDocumentFolderPath() + hatPath) == false)
	{

		myPlayersCurrentHat.Add(-1);
		myPlayersCurrentHat.Add(-1);
		for (auto it = myHats.begin(); it != myHats.end(); ++it)
		{
			myHatsStatus.Add(0);
		}
		std::ofstream file;
		file.open(CU::GetMyDocumentFolderPath() + hatPath, std::ios::binary | std::ios::out);
		//skapa up filen korrekt

		for (int i = 0; i < myPlayersCurrentHat.Size(); ++i)
		{
			file << myPlayersCurrentHat[i];
			file << std::endl;
		}

		for (int i = 0; i < myHatsStatus.Size(); ++i)
		{
			file << myHatsStatus[i];
			file << std::endl;
		}
		file.close();
	}
	else
	{
		std::ifstream file;
		file.open(CU::GetMyDocumentFolderPath() + hatPath, std::ios::binary | std::ios::in);
		int playerOneHat;
		int playerTwoHat;
		file >> playerOneHat;
		file >> playerTwoHat;
		myPlayersCurrentHat.Add(playerOneHat);
		myPlayersCurrentHat.Add(playerTwoHat);

		for (int i = 0; i < myHats.size(); ++i)
		{
			int hasUnlocked;
			file >> hasUnlocked;
			myHatsStatus.Add(hasUnlocked);
		}
		file.close();
	}
	

}

void HatManager::UnlockHat(int aID)
{
	std::string hatPath = "Data/UnlockedHats.bin";
	std::ofstream file;
	file.open(CU::GetMyDocumentFolderPath() + hatPath, std::ios::binary | std::ios::out);

	for (int i = 0; i < myPlayersCurrentHat.Size(); ++i)
	{
		file << myPlayersCurrentHat[i];
	}

	myHatsStatus[aID] = 1;

	for (int i = 0; i < myHatsStatus.Size(); ++i)
	{
		file << myHatsStatus[i];
	}

	file.close();
}

void HatManager::SetHatOnPlayer(int aPlayerID, int aHatID)
{
	myPlayersCurrentHat[aPlayerID - 1] = aHatID;
}

int HatManager::GetHatIDOnPlayer(int aPlayerID) const
{
	return myPlayersCurrentHat[aPlayerID - 1];
}

Prism::ModelProxy* HatManager::GetHat(int aID)
{
	DL_ASSERT_EXP(myHats.find(aID) != myHats.end(), "Hat with id: " + std::to_string(aID) + " not found");

	return myHats[aID];
}

bool HatManager::IsHatUnlocked(int aID) const
{
	return myHatsStatus[aID] != 0;
}

HatManager::HatManager()
	: myPlayersCurrentHat(4)
	, myHatsStatus(8)
{ 
}


HatManager::~HatManager()
{
}