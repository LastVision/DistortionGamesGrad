#include "stdafx.h"

#include <BounceComponent.h>
#include <EffectContainer.h>
#include <Engine.h>
#include <EntityFactory.h>
#include <Entity.h>
#include "Level.h"
#include "LevelFactory.h"
#include <PhysicsInterface.h>
#include <SawBladeComponent.h>
#include <XMLReader.h>
#include <SteamComponent.h>
#include <TriggerComponent.h>

LevelFactory::LevelFactory(const std::string& aLevelListPath, Prism::Camera& aCamera, int aLevel)
	: myCamera(aCamera)
	, myCurrentLevelID(aLevel)
	, myFinalLevelID(0)
{
	ReadLevelList(aLevelListPath);
}

LevelFactory::~LevelFactory()
{
}

bool LevelFactory::LoadLevel(Level*& aLevelOut)
{
	myCurrentLevelID++;
	if (myCurrentLevelID > myFinalLevelID)
	{
		return false;
	}

	DL_ASSERT_EXP(myLevelPaths.find(myCurrentLevelID) != myLevelPaths.end(), "[LevelFactory]: Non-existing ID in LoadLevel! ID most correspond with LI_level.xml");

	aLevelOut = LoadCurrentLevel();
	return true;
}

Level* LevelFactory::LoadCurrentLevel()
{
	Level* level = ReadLevel(myLevelPaths[myCurrentLevelID]);

#ifdef THREAD_PHYSICS
	Prism::PhysicsInterface::GetInstance()->InitThread();
#endif

	return level;
}

void LevelFactory::ReadLevelList(const std::string& aLevelListPath)
{
	myLevelPaths.clear();
	XMLReader reader;
	reader.OpenDocument(aLevelListPath);

	std::string levelPath = "";
	int ID = -1;

	tinyxml2::XMLElement* rootElement = reader.ForceFindFirstChild("root");
	for (tinyxml2::XMLElement* element = reader.FindFirstChild(rootElement); element != nullptr; element = reader.FindNextElement(element))
	{
		reader.ForceReadAttribute(element, "ID", ID);
		reader.ForceReadAttribute(element, "path", levelPath);
		myLevelPaths[ID] = levelPath;

		myFinalLevelID = max(myFinalLevelID, ID);
	}
	reader.CloseDocument();
}

Level* LevelFactory::ReadLevel(const std::string& aLevelPath)
{
	XMLReader reader;
	reader.OpenDocument(aLevelPath);

	tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("root");
	levelElement = reader.ForceFindFirstChild(levelElement, "scene");

	Level* level = new Level(myCamera);

	LoadLevelData(level, reader, levelElement);
	LoadStartAndGoal(level, reader, levelElement);
	LoadProps(level, reader, levelElement);
	LoadSpikes(level, reader, levelElement);
	LoadSawBlades(level, reader, levelElement);
	LoadSteamVents(level, reader, levelElement);
	LoadBouncers(level, reader, levelElement);

	reader.CloseDocument();

	level->CreatePlayers();
	return level;
}

void LevelFactory::LoadLevelData(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	tinyxml2::XMLElement* levelDataElement = aReader.ForceFindFirstChild(aElement, "levelData");

	std::string cubeMap;
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(levelDataElement, "cubemap"), "source", cubeMap);
	Prism::EffectContainer::GetInstance()->SetCubeMap(cubeMap);
}

void LevelFactory::LoadProps(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement)
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

		aLevel->Add(EntityFactory::CreateEntity(eEntityType::PROP, CU::ToLower(propType)
			, aLevel->GetScene(), propPosition, propRotation, propScale));
	}
}

void LevelFactory::LoadSpikes(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement)
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

		aLevel->Add(EntityFactory::CreateEntity(eEntityType::SPIKE, CU::ToLower(spikeType),
			aLevel->GetScene(), spikePosition, spikeRotation, spikeScale));
	}
}

void LevelFactory::LoadSawBlades(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement)
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

		Entity* entity(EntityFactory::CreateEntity(eEntityType::SAW_BLADE, CU::ToLower(sawBladeType),
			aLevel->GetScene(), sawBladePosition, sawBladeRotation, sawBladeScale));

		tinyxml2::XMLElement* patrolElement = aReader.FindFirstChild(entityElement, "patrols");

		if (patrolElement != nullptr)
		{
			float speed = 0.f;
			float delay = 0.f;

			aReader.ForceReadAttribute(entityElement, "sawBladeType", sawBladeType);

			CU::GrowingArray<CU::Vector3<float>> patrolPositions(8);

			for (tinyxml2::XMLElement* patrolPositionElement = aReader.FindFirstChild(patrolElement, "patrolLocation"); patrolPositionElement != nullptr;
				patrolPositionElement = aReader.FindNextElement(patrolPositionElement, "patrolLocation"))
			{
				CU::Vector3<float> position;
				aReader.ForceReadAttribute(patrolPositionElement, "X", "Y", "Z", position);
				patrolPositions.Add(position);
			}
			tinyxml2::XMLElement* patrolSpeedElement = aReader.FindFirstChild(entityElement, "patrolSpeed");
			tinyxml2::XMLElement* patrolDelayElement = aReader.FindFirstChild(entityElement, "patrolDelay");
			
			DL_ASSERT_EXP(entity->GetComponent<SawBladeComponent>() != nullptr, "Saw blades have to have saw blade components to be able to patrol");
			DL_ASSERT_EXP(patrolSpeedElement != nullptr, "SawBlade has to have a patrolSpeed in level XML");

			aReader.ForceReadAttribute(patrolSpeedElement, "value", speed);

			if (patrolDelayElement != nullptr)
			{
				aReader.ForceReadAttribute(patrolDelayElement, "value", delay);
			}

			entity->GetComponent<SawBladeComponent>()->SetPatrol(patrolPositions, speed, delay);
		}
		aLevel->Add(entity);
	}
}

void LevelFactory::LoadSteamVents(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aElement, "steamVent"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "steamVent"))
	{
		std::string steamVentType;
		CU::Vector3f steamVentPosition;
		CU::Vector3f steamVentRotation;
		CU::Vector3f steamVentScale;
		float force = 0.f;

		aReader.ForceReadAttribute(entityElement, "steamVentType", steamVentType);

		ReadOrientation(aReader, entityElement, steamVentPosition, steamVentRotation, steamVentScale);

		Entity* entity(EntityFactory::CreateEntity(eEntityType::STEAM_VENT, CU::ToLower(steamVentType),
			aLevel->GetScene(), steamVentPosition, steamVentRotation, steamVentScale));
		
		tinyxml2::XMLElement* forceElement = aReader.FindFirstChild(entityElement, "force");
		tinyxml2::XMLElement* steamTimeElement = aReader.FindFirstChild(entityElement, "steamTime");
		tinyxml2::XMLElement* steamIntervalElement = aReader.FindFirstChild(entityElement, "steamInterval");
		tinyxml2::XMLElement* steamDelayElement = aReader.FindFirstChild(entityElement, "steamDelay");

		DL_ASSERT_EXP(entity->GetComponent<SteamComponent>() != nullptr, "Steam vents need steam components to work");
		DL_ASSERT_EXP(forceElement != nullptr, "Steam vents need force in level XML");

		aReader.ForceReadAttribute(forceElement, "value", force);

		entity->GetComponent<SteamComponent>()->SetForce(force);

		if (steamTimeElement != nullptr && steamIntervalElement != nullptr)
		{
			float steamTime = 0.f;
			float steamInterval = 0.f;
			float steamDelay = 0.f;

			aReader.ForceReadAttribute(steamTimeElement, "value", steamTime);
			aReader.ForceReadAttribute(steamIntervalElement, "value", steamInterval);

			if (steamDelayElement != nullptr)
			{
				aReader.ForceReadAttribute(steamDelayElement, "value", steamDelay);
			}

			entity->GetComponent<SteamComponent>()->SetSteamVariables(steamInterval, steamTime, steamDelay);

		}
		aLevel->Add(entity);
	}
}

void LevelFactory::LoadBouncers(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aElement, "bouncer"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "bouncer"))
	{
		std::string bouncerType;
		CU::Vector3f bouncerPosition;
		CU::Vector3f bouncerRotation;
		CU::Vector3f bouncerScale;
		float force = 0.f;

		aReader.ForceReadAttribute(entityElement, "bouncerType", bouncerType);

		ReadOrientation(aReader, entityElement, bouncerPosition, bouncerRotation, bouncerScale);

		Entity* entity = EntityFactory::CreateEntity(eEntityType::BOUNCER, CU::ToLower(bouncerType),
			aLevel->GetScene(), bouncerPosition, bouncerRotation, bouncerScale);

		tinyxml2::XMLElement* forceElement = aReader.FindFirstChild(entityElement, "force");

		DL_ASSERT_EXP(entity->GetComponent<BounceComponent>() != nullptr, "Bouncer need bounce component to work");
		DL_ASSERT_EXP(forceElement != nullptr, "Bouncer has to have a force in level XML");

		aReader.ForceReadAttribute(forceElement, "value", force);

		entity->GetComponent<BounceComponent>()->SetForce(force);

		aLevel->Add(entity);
	}
}

void LevelFactory::LoadStartAndGoal(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	tinyxml2::XMLElement* spawnElement = aReader.ForceFindFirstChild(aElement, "spawnPoint");
	tinyxml2::XMLElement* goalElement = aReader.ForceFindFirstChild(aElement, "goalPoint");

	CU::Vector3f position;
	CU::Vector3f rotation;
	CU::Vector3f scale;

	ReadOrientation(aReader, spawnElement, position, rotation, scale);

	aLevel->Add(EntityFactory::CreateEntity(eEntityType::SPAWN_POINT,
		aLevel->GetScene(), position, rotation, scale));
	aLevel->SetSpawnPosition(position);

	ReadOrientation(aReader, goalElement, position, rotation, scale);
	int levelID = 0;
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(goalElement, "levelid"), "id", levelID);

	Entity* entity = EntityFactory::CreateEntity(eEntityType::GOAL_POINT,
		aLevel->GetScene(), position, rotation, scale);
	entity->GetComponent<TriggerComponent>()->SetLevelChangeID(levelID);

	aLevel->Add(entity);

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