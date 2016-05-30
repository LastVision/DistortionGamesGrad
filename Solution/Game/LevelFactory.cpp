#include "stdafx.h"

#include <AcidComponent.h>
#include <BounceComponent.h>
#include <DirectionalLight.h>
#include <EffectContainer.h>
#include <Engine.h>
#include <EntityFactory.h>
#include <Entity.h>
#include <GameConstants.h>
#include "Level.h"
#include "LevelFactory.h"
#include <PhysicsInterface.h>
#include <SawBladeComponent.h>
#include "ScoreInfo.h"
#include <SteamComponent.h>
#include <StomperComponent.h>
#include <TriggerComponent.h>
#include <XMLReader.h>
#include <PointLight.h>
#include <SpotLight.h>

LevelFactory::LevelFactory(const std::string& aLevelListPath
	, const std::string& aNightLevelListPath, Prism::Camera& aCamera, int aLevel)
	: myCamera(aCamera)
	, myCurrentLevelID(aLevel)
	, myFinalLevelID(0)
	, myFinalNightmareLevelID(0)
	, myHasCreatedUnlockedLevels(true)
{
	ReadLevelList(aLevelListPath, myLevelPaths, GC::TotalLevels, myFinalLevelID, false);
	ReadLevelList(aNightLevelListPath, myNightmareLevelPaths, GC::TotalNightmareLevels, myFinalNightmareLevelID, true);
}

LevelFactory::~LevelFactory()
{
}

bool LevelFactory::LoadLevel(Level*& aLevelOut)
{
	myCurrentLevelID++;

	if (GC::NightmareMode == false)
	{
		if (myCurrentLevelID > myFinalLevelID)
		{
			return false;
		}
		DL_ASSERT_EXP(myLevelPaths.find(myCurrentLevelID) != myLevelPaths.end()
			, "[LevelFactory]: Non-existing ID in LoadLevel! ID most correspond with LI_level.xml");
	}
	else
	{
		if (myCurrentLevelID > myFinalNightmareLevelID)
		{
			return false;
		}
		DL_ASSERT_EXP(myNightmareLevelPaths.find(myCurrentLevelID) != myNightmareLevelPaths.end()
			, "[LevelFactory]: Non-existing ID in LoadLevel nightmare! ID most correspond with LI_level_nightmare.xml");
	}


	aLevelOut = LoadCurrentLevel();
	return true;
}

void LevelFactory::RestartLevel()
{
	myCurrentLevelID--;
}

Level* LevelFactory::LoadCurrentLevel()
{
	Level* level = nullptr;

	if (GC::NightmareMode == false)
	{
		level = ReadLevel(myLevelPaths[myCurrentLevelID]);
	}
	else
	{
		level = ReadLevel(myNightmareLevelPaths[myCurrentLevelID]);
	}

#ifdef THREAD_PHYSICS
	Prism::PhysicsInterface::GetInstance()->InitThread();
#endif

	return level;
}

void LevelFactory::ReadLevelList(const std::string& aLevelListPath, std::unordered_map<int, std::string>& aLevelMap
	, int& aTotalLevels, int& aFinalLevel, bool aIsNightmare)
{
	aFinalLevel = 0;
	aTotalLevels = 0;

	aLevelMap.clear();
	XMLReader reader;
	reader.OpenDocument(aLevelListPath);

	std::string levelPath = "";
	int ID = -1;

	tinyxml2::XMLElement* rootElement = reader.ForceFindFirstChild("root");
	for (tinyxml2::XMLElement* element = reader.FindFirstChild(rootElement); element != nullptr; element = reader.FindNextElement(element))
	{
		reader.ForceReadAttribute(element, "ID", ID);
		reader.ForceReadAttribute(element, "path", levelPath);
		AddLevelToUnlockedLevelFile(ID, aIsNightmare);
		aLevelMap[ID] = levelPath;

		aFinalLevel = max(aFinalLevel, ID);
		aTotalLevels++;
	}

	myHasCreatedUnlockedLevels = true;
	reader.CloseDocument();
}

Level* LevelFactory::ReadLevel(const std::string& aLevelPath)
{
	XMLReader reader;
	reader.OpenDocument(aLevelPath);

	tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("root");
	levelElement = reader.ForceFindFirstChild(levelElement, "scene");

	Level* level = new Level(myCamera, myCurrentLevelID);
	LoadLevelData(level, reader, levelElement);
	LoadStartAndGoal(level, reader, levelElement);
	LoadProps(level, reader, levelElement);
	//LoadSpikes(level, reader, levelElement);
	//LoadSawBlades(level, reader, levelElement);
	//LoadSteamVents(level, reader, levelElement);
	//LoadBouncers(level, reader, levelElement);
	//LoadStompers(level, reader, levelElement);
	//LoadAcid(level, reader, levelElement);
	LoadPointLights(level, reader, levelElement);
	LoadDirectionalLights(level, reader, levelElement);
	LoadSpotLights(level, reader, levelElement);

	level->CreatePlayers();

	reader.CloseDocument();

	
	return level;
}

void LevelFactory::LoadLevelData(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	tinyxml2::XMLElement* levelDataElement = aReader.ForceFindFirstChild(aElement, "levelData");

	std::string cubeMap;
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(levelDataElement, "cubemap"), "source", cubeMap);
	Prism::EffectContainer::GetInstance()->SetCubeMap(cubeMap);

	float shortTime;
	float mediumTime;
	float longTime;

	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(levelDataElement, "time"), "short", shortTime);
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(levelDataElement, "time"), "medium", mediumTime);
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(levelDataElement, "time"), "long", longTime);

	aLevel->CreateScoreInfo(shortTime, mediumTime, longTime);
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
		tinyxml2::XMLElement* steamSubTypeElement = aReader.FindFirstChild(entityElement, "steamSubType");

		DL_ASSERT_EXP(entity->GetComponent<SteamComponent>() != nullptr, "Steam vents need steam components to work");
		DL_ASSERT_EXP(forceElement != nullptr, "Steam vents need force in level XML");

		aReader.ForceReadAttribute(forceElement, "value", force);


		if (steamTimeElement != nullptr && steamIntervalElement != nullptr)
		{
			std::string steamType = "";
			float steamTime = 0.f;
			float steamInterval = 0.f;
			float steamDelay = 0.f;

			aReader.ForceReadAttribute(steamTimeElement, "value", steamTime);
			aReader.ForceReadAttribute(steamIntervalElement, "value", steamInterval);
			aReader.ForceReadAttribute(steamSubTypeElement, "type", steamType);

			if (steamDelayElement != nullptr)
			{
				aReader.ForceReadAttribute(steamDelayElement, "value", steamDelay);
			}

			entity->GetComponent<SteamComponent>()->InitSteam(aLevel->GetScene(), steamVentRotation, steamType, steamInterval, steamTime, steamDelay);

		}

		entity->GetComponent<SteamComponent>()->SetForce(force);

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

void LevelFactory::LoadStompers(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aElement, "stomper_holder"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "stomper_holder"))
	{
		std::string stomperType;
		CU::Vector3f stomperPosition;
		CU::Vector3f stomperRotation;
		CU::Vector3f stomperScale;
		float timeBeforeStomp;
		float timeStomperDown;
		float stompSpeedOut;
		float stompSpeedIn;
		float distance;
		float delayBeforeStomp;

		aReader.ForceReadAttribute(entityElement, "stomperType", stomperType);

		ReadOrientation(aReader, entityElement, stomperPosition, stomperRotation, stomperScale);

		Entity* entity(EntityFactory::CreateEntity(eEntityType::STOMPER_HOLDER, CU::ToLower(stomperType),
			aLevel->GetScene(), stomperPosition, stomperRotation, stomperScale));

		aReader.ForceReadAttribute(aReader.FindFirstChild(entityElement, "timeBeforeStomp"), "value", timeBeforeStomp);
		aReader.ForceReadAttribute(aReader.FindFirstChild(entityElement, "timeStomperIsDown"), "value", timeStomperDown);
		aReader.ForceReadAttribute(aReader.FindFirstChild(entityElement, "stompSpeedOut"), "value", stompSpeedOut);
		aReader.ForceReadAttribute(aReader.FindFirstChild(entityElement, "stompSpeedIn"), "value", stompSpeedIn);
		aReader.ForceReadAttribute(aReader.FindFirstChild(entityElement, "distance"), "value", distance);
		aReader.ForceReadAttribute(aReader.FindFirstChild(entityElement, "timeDelay"), "value", delayBeforeStomp);

		DL_ASSERT_EXP(entity->GetComponent<StomperComponent>() != nullptr, "stomper holders need stomper components to work");

		entity->GetComponent<StomperComponent>()->InitStomper(timeBeforeStomp, timeStomperDown
			, stompSpeedOut, stompSpeedIn, distance, delayBeforeStomp);

		aLevel->Add(entity);
	}
}

void LevelFactory::LoadAcid(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aElement, "acid"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "acid"))
	{
		std::string acidType;
		CU::Vector3f acidPosition;
		CU::Vector3f acidRotation;
		CU::Vector3f acidScale;
		int amount = 0;
		float maxInterval = 0.f;
		float minInterval = 0.f;
		CU::Vector3<float> maxVelocity;
		CU::Vector3<float> minVelocity;

		aReader.ForceReadAttribute(entityElement, "acidType", acidType);

		ReadOrientation(aReader, entityElement, acidPosition, acidRotation, acidScale);

		Entity* entity = EntityFactory::CreateEntity(eEntityType::ACID, CU::ToLower(acidType),
			aLevel->GetScene(), acidPosition, acidRotation, acidScale);

		DL_ASSERT_EXP(entity->GetComponent<AcidComponent>() != nullptr, "Acid need acid component to work");

		aReader.ForceReadAttribute(aReader.FindFirstChild(entityElement, "amount"), "value", amount);
		aReader.ForceReadAttribute(aReader.FindFirstChild(entityElement, "maxInterval"), "value", maxInterval);
		aReader.ForceReadAttribute(aReader.FindFirstChild(entityElement, "minInterval"), "value", minInterval);
		aReader.ForceReadAttribute(aReader.FindFirstChild(entityElement, "maxVelocity"), "X", "Y", "Z", maxVelocity);
		aReader.ForceReadAttribute(aReader.FindFirstChild(entityElement, "minVelocity"), "X", "Y", "Z", minVelocity);

		entity->GetComponent<AcidComponent>()->InitAcid(amount, maxInterval, minInterval
			, maxVelocity, minVelocity, aLevel->GetScene());

		aLevel->Add(entity);
	}
}

void LevelFactory::LoadPointLights(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* lightElement = aReader.FindFirstChild(aElement, "pointlight"); lightElement != nullptr;
		lightElement = aReader.FindNextElement(lightElement, "pointlight"))
	{
		CU::Vector3<float> position;
		CU::Vector4<float> color;
		float range;

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "position"), "X", position.x);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "position"), "Y", position.y);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "position"), "Z", position.z);

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "R", color.x);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "G", color.y);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "B", color.z);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "A", color.w);

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "range"), "value", range);

		Prism::PointLight* light = new Prism::PointLight(false);
		light->SetPosition(position);
		light->SetColor(color);
		light->SetRange(range);
		light->Update();
		aLevel->Add(light);
	}
}

void LevelFactory::LoadDirectionalLights(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* lightElement = aReader.FindFirstChild(aElement, "directionallight"); lightElement != nullptr;
		lightElement = aReader.FindNextElement(lightElement, "directionallight"))
	{
		CU::Vector3<float> direction;
		CU::Vector4<float> color;

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "direction"), "X", direction.x);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "direction"), "Y", direction.y);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "direction"), "Z", direction.z);

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "R", color.x);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "G", color.y);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "B", color.z);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "A", color.w);

		Prism::DirectionalLight* light = new Prism::DirectionalLight();
		light->SetColor(color);
		light->SetDir(CU::Vector4<float>(direction, 0.f));
		aLevel->Add(light);
	}
}

void LevelFactory::LoadSpotLights(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	for (tinyxml2::XMLElement* lightElement = aReader.FindFirstChild(aElement, "spotlight"); lightElement != nullptr;
		lightElement = aReader.FindNextElement(lightElement, "spotlight"))
	{
		CU::Vector3<float> position;
		CU::Vector3<float> rotation;
		CU::Vector4<float> color;
		float range;
		float spotangle;


		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "position"), "X", "Y", "Z", position);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "rotation"), "X", "Y", "Z", rotation);

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "R", color.x);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "G", color.y);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "B", color.z);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "color"), "A", color.w);

		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "range"), "value", range);
		aReader.ForceReadAttribute(aReader.ForceFindFirstChild(lightElement, "spotangle"), "value", spotangle);

		unsigned int gid(UINT32_MAX);
		Prism::SpotLight* light = new Prism::SpotLight(gid);
		light->SetPosition(CU::Vector4<float>(position, 1.f));
		light->SetColor(color);
		light->SetRange(range);
		light->SetAngle(CU::Math::DegreeToRad(spotangle / 2.f));

		rotation.x = CU::Math::DegreeToRad(rotation.x);
		rotation.y = CU::Math::DegreeToRad(rotation.y);
		rotation.z = CU::Math::DegreeToRad(rotation.z);

		light->PerformTransformation(CU::Matrix44f::CreateRotateAroundZ(rotation.z));
		light->PerformTransformation(CU::Matrix44f::CreateRotateAroundX(rotation.x));
		light->PerformTransformation(CU::Matrix44f::CreateRotateAroundY(rotation.y));

		light->Update();
		aLevel->Add(light);
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

	CU::Vector2<float> spawnVelocity;
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(spawnElement, "velocity"), "X", spawnVelocity.x);
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(spawnElement, "velocity"), "Y", spawnVelocity.y);
	aLevel->SetSpawnVelocity(spawnVelocity);
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

void LevelFactory::AddLevelToUnlockedLevelFile(const int aLevelID, bool aIsNightmare)
{
	std::fstream file;
	std::string path = "Data/UnlockedLevels.bin";

	if (aIsNightmare == true)
	{
		path = "Data/UnlockedLevels_Nightmare.bin";
	}

	file.open(CU::GetMyDocumentFolderPath() + path, std::ios::binary | std::ios::in);
	if (file.peek() == std::ifstream::traits_type::eof())
	{
		myHasCreatedUnlockedLevels = false;
	}
	file.close();
	if (myHasCreatedUnlockedLevels == false)
	{
		std::ios::openmode mode = std::ios::binary | std::ios::app | std::ios::out;
		if (aLevelID == 1)
		{
			mode = std::ios::binary | std::ios::out;
		}
		file.open(CU::GetMyDocumentFolderPath() + path, mode);
		if (file.is_open() == true)
		{
			if (myHasCreatedUnlockedLevels == false)
			{
				if (aLevelID == 1)
				{
					file << aLevelID << std::endl << true;
				}
				else
				{
					file << std::endl << aLevelID << std::endl << false;
				}
			}
		}
		file.close();
	}
}