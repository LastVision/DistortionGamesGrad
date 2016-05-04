#include <CommonHelper.h>
#include <Defines.h>
#include <MathHelper.h>
#include "ModelCountReader.h"
#include <XMLReader.h>
#include <Vertices.h>
#include <iostream>
#include <MemoryMacros.h>

ModelCountReader::ModelCountReader()
{
}


ModelCountReader::~ModelCountReader()
{
}

void ModelCountReader::ReadFile(const std::string& aFilePath)
{
	XMLReader reader;
	reader.OpenDocument(aFilePath);
	tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("root");

	levelElement = reader.ForceFindFirstChild(levelElement, "level");

	while (levelElement != nullptr)
	{
		std::string path;
		reader.ForceReadAttribute(levelElement, "path", path);

		ReadLevel(path);

		levelElement = reader.FindNextElement(levelElement, "level");
	}

	reader.CloseDocument();


	SaveLevel();
}

void ModelCountReader::ReadLevel(const std::string& aFilePath)
{
	XMLReader reader;
	reader.OpenDocument(aFilePath);
	tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("root");
	levelElement = reader.ForceFindFirstChild(levelElement, "scene");

	LoadProps(reader, levelElement);

	SyncLevels();

	reader.CloseDocument();
}

void ModelCountReader::LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement)
{
	tinyxml2::XMLElement* propElement = aReader.FindFirstChild(aLevelElement, "prop");
	for (; propElement != nullptr; propElement = aReader.FindNextElement(propElement, "prop"))
	{
		std::string propType;
		aReader.ForceReadAttribute(propElement, "propType", propType);

		if (myProps.find(propType) == myProps.end())
		{
			myProps[propType] = 0;
		}
		++myProps[propType];
	}

	tinyxml2::XMLElement* artifactElement = aReader.FindFirstChild(aLevelElement, "artifact");
	for (; artifactElement != nullptr; artifactElement = aReader.FindNextElement(artifactElement, "artifact"))
	{
		std::string type;
		aReader.ForceReadAttribute(artifactElement, "type", type);

		if (myProps.find(type) == myProps.end())
		{
			myProps[type] = 0;
		}
		++myProps[type];
	}
}

void ModelCountReader::AddUnique(const std::string& aName, int aCount)
{
	if (myProps.find(aName) == myProps.end())
	{
		myProps[aName] = aCount;
	}
	else
	{
		myProps[aName] += aCount;
	}
}

void ModelCountReader::SyncLevels()
{
	for (auto it = myProps.begin(); it != myProps.end(); ++it)
	{
		if (myFinalProps.find(it->first) == myFinalProps.end())
		{
			myFinalProps[it->first] = 0;
		}

		int current = it->second;
		int finalCount = myFinalProps[it->first];

		if (current > finalCount)
		{
			myFinalProps[it->first] = current;
		}
	}

	myProps.clear();
}

void ModelCountReader::SaveLevel()
{
	std::string outputPath = "GeneratedData/modelcount.bin";
	CU::BuildFoldersInPath(outputPath);


	std::fstream file;
	file.open(outputPath, std::ios::out);
	
	for (auto it = myFinalProps.begin(); it != myFinalProps.end(); ++it)
	{
		file << it->first << " " << it->second << std::endl;
	}

	file.close();
}