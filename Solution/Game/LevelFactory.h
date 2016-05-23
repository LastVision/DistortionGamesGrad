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
	LevelFactory(const std::string& aLevelListPath, Prism::Camera& aCamera, int aLevel);
	~LevelFactory();

	bool LoadLevel(Level*& aLevelOut);

	void RestartLevel();

private:
	void operator=(LevelFactory&) = delete;
	Level* LoadCurrentLevel();

	void ReadLevelList(const std::string& aLevelListPath);
	Level* ReadLevel(const std::string& aLevelPath);

	void LoadLevelData(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadProps(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadSpikes(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadSawBlades(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadSteamVents(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadBouncers(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);

	void LoadStartAndGoal(Level* aLevel, XMLReader& aReader, tinyxml2::XMLElement* aElement);

	void ReadOrientation(XMLReader& aReader, tinyxml2::XMLElement* aElement, CU::Vector3f& aPosition, CU::Vector3f& aRotation, CU::Vector3f& aScale);

	Prism::Camera& myCamera;
	
	int myCurrentLevelID;
	int myFinalLevelID;

	std::unordered_map<int, std::string> myLevelPaths;
};

