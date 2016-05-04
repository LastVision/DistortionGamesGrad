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
	LevelFactory(const std::string& aLevelListPath, Prism::Camera& aCamera);
	~LevelFactory();

	Level* LoadLevel(const int& aLevelID);
	Level* LoadCurrentLevel();
	Level* LoadNextLevel();
private:
	void operator=(LevelFactory&) = delete;

	void ReadLevelList(const std::string& aLevelListPath);
	void ReadLevel(const std::string& aLevelPath);

	void LoadLevelData(XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadSpikes(XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadSawBlades(XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadSteam(XMLReader& aReader, tinyxml2::XMLElement* aElement);
	void LoadBouncers(XMLReader& aReader, tinyxml2::XMLElement* aElement);

	void LoadStartAndGoal(XMLReader& aReader, tinyxml2::XMLElement* aElement);

	void ReadOrientation(XMLReader& aReader, tinyxml2::XMLElement* aElement, CU::Vector3f& aPosition, CU::Vector3f& aRotation, CU::Vector3f& aScale);

	Prism::Camera& myCamera;

	Level* myCurrentLevel;
	int myCurrentLevelID;

	std::unordered_map<int, std::string> myLevelPaths;
};

