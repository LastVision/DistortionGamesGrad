#include "stdafx.h"

#include <EffectContainer.h>
#include <Engine.h>
#include <EntityFactory.h>
#include "Level.h"
#include "LevelFactory.h"
#include <XMLReader.h>

LevelFactory::LevelFactory(const std::string& aLevelListPath, Prism::Camera& aCamera)
	: myCamera(aCamera)
	, myCurrentLevel(nullptr)
	, myCurrentLevelID(0)
{
	ReadLevelList(aLevelListPath);
}

LevelFactory::~LevelFactory()
{
}

Level* LevelFactory::LoadLevel(const int& aLevelID)
{
	DL_ASSERT_EXP(myLevelPaths.find(aLevelID) != myLevelPaths.end(), "[LevelFactory]: Non-existing ID in LoadLevel! ID most correspond with LI_level.xml");
	myCurrentLevelID = aLevelID;

	return LoadCurrentLevel();
}

Level* LevelFactory::LoadCurrentLevel()
{
	SAFE_DELETE(myCurrentLevel);

	myCurrentLevel = new Level(myCamera);

	ReadLevel(myLevelPaths[myCurrentLevelID]);

	return myCurrentLevel;
}

Level* LevelFactory::LoadNextLevel()
{
	myCurrentLevelID++;

	return LoadCurrentLevel();
}

void LevelFactory::ReadLevelList(const std::string& aLevelListPath)
{
	myLevelPaths.clear();
	XMLReader reader;
	reader.OpenDocument(aLevelListPath);

	std::string levelPath = "";
	int ID = -1;
	int lastID = ID - 1;

	tinyxml2::XMLElement* rootElement = reader.ForceFindFirstChild("root");
	for (tinyxml2::XMLElement* element = reader.FindFirstChild(rootElement); element != nullptr; element = reader.FindNextElement(element))
	{
		lastID = ID;

		reader.ForceReadAttribute(element, "ID", ID);
		reader.ForceReadAttribute(element, "path", levelPath);

		DL_ASSERT_EXP(ID - 1 == lastID, "[LevelFactory]: Wrong ID-number in LI_level.xml! The numbers should be counting up, in order.");

		myLevelPaths[ID] = levelPath;
	}
	reader.CloseDocument();
}

void LevelFactory::ReadLevel(const std::string& aLevelPath)
{
	XMLReader reader;
	reader.OpenDocument(aLevelPath);

	tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("root");
	levelElement = reader.ForceFindFirstChild(levelElement, "scene");

	std::string cubeMap;
	reader.ForceReadAttribute(reader.ForceFindFirstChild(levelElement, "cubemap"), "source", cubeMap);
	Prism::EffectContainer::GetInstance()->SetCubeMap(cubeMap);

	LoadProps(reader, levelElement);
	LoadSpikes(reader, levelElement);
	LoadSawBlades(reader, levelElement);

	reader.CloseDocument();
}

void LevelFactory::LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aElement, "prop"); entityElement != nullptr; 
		entityElement = aReader.FindNextElement(entityElement, "prop"))
	{
		std::string propType;
		CU::Vector3f propPosition;
		CU::Vector3f propRotation;
		CU::Vector3f propScale;

		aReader.ForceReadAttribute(entityElement, "propType", propType);

		ReadOrientation(aReader, entityElement, propPosition, propRotation, propScale);

		myCurrentLevel->myEntities.Add(EntityFactory::CreateEntity(eEntityType::PROP, propType, 
			myCurrentLevel->myScene, propPosition, propRotation, propScale));
		myCurrentLevel->myEntities.GetLast()->AddToScene();
		myCurrentLevel->myEntities.GetLast()->Reset();
	}
}

void LevelFactory::LoadSpikes(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aElement, "spike"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "spike"))
	{
		std::string spikeType;
		CU::Vector3f spikePosition;
		CU::Vector3f spikeRotation;
		CU::Vector3f spikeScale;

		aReader.ForceReadAttribute(entityElement, "spikeType", spikeType);

		ReadOrientation(aReader, entityElement, spikePosition, spikeRotation, spikeScale);

		myCurrentLevel->myEntities.Add(EntityFactory::CreateEntity(eEntityType::SPIKE, spikeType,
			myCurrentLevel->myScene, spikePosition, spikeRotation, spikeScale));
		myCurrentLevel->myEntities.GetLast()->AddToScene();
		myCurrentLevel->myEntities.GetLast()->Reset();
	}
}

void LevelFactory::LoadSawBlades(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aElement, "sawBlade"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "sawBlade"))
	{
		std::string sawBladeType;
		CU::Vector3f sawBladePosition;
		CU::Vector3f sawBladeRotation;
		CU::Vector3f sawBladeScale;

		aReader.ForceReadAttribute(entityElement, "sawBladeType", sawBladeType);

		ReadOrientation(aReader, entityElement, sawBladePosition, sawBladeRotation, sawBladeScale);

		myCurrentLevel->myEntities.Add(EntityFactory::CreateEntity(eEntityType::SPIKE, sawBladeType,
			myCurrentLevel->myScene, sawBladePosition, sawBladeRotation, sawBladeScale));
		myCurrentLevel->myEntities.GetLast()->AddToScene();
		myCurrentLevel->myEntities.GetLast()->Reset();
	}
}

void LevelFactory::ReadOrientation(XMLReader& aReader, tinyxml2::XMLElement* aElement, CU::Vector3f& aPosition, CU::Vector3f& aRotation, CU::Vector3f& aScale)
{
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(aElement, "position"), "X", "Y", "Z", aPosition);
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(aElement, "rotation"), "X", "Y", "Z", aRotation);
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(aElement, "scale"), "X", "Y", "Z", aScale);

	aRotation.x = CU::Math::DegreeToRad(aRotation.x);
	aRotation.y = CU::Math::DegreeToRad(aRotation.y);
	aRotation.z = CU::Math::DegreeToRad(aRotation.z);
}