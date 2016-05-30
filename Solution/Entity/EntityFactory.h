#pragma once
#include <EngineEnums.h>
#include <FileWatcher.h>
class ComponentLoader;
class Entity;
class XMLReader;

namespace tinyxml2
{
	class XMLElement;
}

namespace Prism
{
	class Scene;
	class Terrain;
}

class EntityFactory
{
public:
	static EntityFactory* GetInstance();
	static void Destroy();

	void UpdateFileWatcher();
	void LoadEntities(const char* aEntityListXML);

	static Entity* CreateEntity(eEntityType aType, Prism::Scene* aScene, const CU::Vector3f& aPosition
		, const CU::Vector3f& aRotation = CU::Vector3f(), const CU::Vector3f& aScale = { 1.f, 1.f, 1.f }, int aPlayerID = -1);

	static Entity* CreateEntity(eEntityType aType, std::string aSubType, Prism::Scene* aScene, const CU::Vector3f& aPosition
		, const CU::Vector3f& aRotation = CU::Vector3f(), const CU::Vector3f& aScale = { 1.f, 1.f, 1.f }, int aPlayerID = -1);

private:

	void LoadEntity(const char* aEntityPath);
	void ReloadEntity(const std::string& aEntityPath);
	void ReadComponents(XMLReader& aReader, tinyxml2::XMLElement* aEntityElement, EntityData& someEntityData);

	EntityFactory();
	~EntityFactory();

	std::unordered_map<eEntityType, EntityData> myLoadedEntityData;
	std::unordered_map<std::string, EntityData> myLoadedSubEntityData;

	ComponentLoader* myComponentLoader;
	CU::FileWatcher myFileWatcher;

	static EntityFactory* myInstance;
};

