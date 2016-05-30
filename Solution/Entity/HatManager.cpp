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
}

Prism::ModelProxy* HatManager::GetHat(int aID)
{
	DL_ASSERT_EXP(myHats.find(aID) != myHats.end(), "Hat with id: " + std::to_string(aID) + " not found");

	return myHats[aID];
}

HatManager::HatManager()
{
}


HatManager::~HatManager()
{
}