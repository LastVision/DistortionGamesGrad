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

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(entityElement, "position"), "X", "Y", "Z", propPosition);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(entityElement, "rotation"), "X", "Y", "Z", propRotation);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(entityElement, "scale"), "X", "Y", "Z", propScale);

		propRotation.x = CU::Math::DegreeToRad(propRotation.x);
		propRotation.y = CU::Math::DegreeToRad(propRotation.y);
		propRotation.z = CU::Math::DegreeToRad(propRotation.z);

		myCurrentLevel->myEntities.Add(EntityFactory::CreateEntity(eEntityType::PROP, propType, 
			myCurrentLevel->myScene, propPosition, propRotation, propScale));
		myCurrentLevel->myEntities.GetLast()->AddToScene();
		myCurrentLevel->myEntities.GetLast()->Reset();
	}
}