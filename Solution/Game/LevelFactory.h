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
	void ReadLevelList(const std::string& aLevelListPath);
	void ReadLevel(const std::string& aLevelPath);

	void LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aElement);

	Prism::Camera& myCamera;

	Level* myCurrentLevel;
	int myCurrentLevelID;

	std::unordered_map<int, std::string> myLevelPaths;
};

