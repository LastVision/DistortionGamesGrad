#pragma once
#include <GrowingArray.h>
#include <unordered_map>

class Level;
class XMLReader;

namespace Prism
{
	class Camera;
};

namespace tinyxml2
{
	class XMLElement;
};

class LevelFactory
{
public:
	LevelFactory(const std::string& aLevelListPath, const std::string& aNightLevelListPath
		, Prism::Camera& aCamera, int aLevel);
	~LevelFactory();

	bool LoadLevel(Level*& aLevelOut);

	void RestartLevel();

private:
	void operator=(LevelFactory&) = delete;
	Level* LoadCurrentLevel();

	void ReadLevelList(const std::string& aLevelListPath, std::unordered_map<int, std::string>& aLevelMap
		, int& aFinalLevel, bool aIsNightmare);
	Level* ReadLevel(const std::string& aLevelPath);

	void LoadLevelData(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadProps(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadSpikes(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadSawBlades(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadSteamVents(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadBouncers(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadStompers(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadAcid(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadPointLights(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadDirectionalLights(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadSpotLights(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);

	void LoadStartAndGoal(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);

	void ReadOrientation(XMLReader& aReader, tinyxml2::XMLElement* aElement, CU::Vector3f& aPosition, CU::Vector3f& aRotation, CU::Vector3f& aScale);

	void AddLevelToUnlockedLevelFile(const int aLevelID, bool aIsNightmare);

	Prism::Camera& myCamera;
	
	int myCurrentLevelID;
	int myFinalLevelID;
	int myFinalNightmareLevelID;

	std::unordered_map<int, std::string> myLevelPaths;
	std::unordered_map<int, std::string> myNightmareLevelPaths;

	bool myHasCreatedUnlockedLevels;
};