#include "stdafx.h"

#include <CommonHelper.h>
#include "ComponentLoader.h"
#include "EntityFactory.h"
#include "EntityEnumConverter.h"
#include <string>

#include "PollingStation.h"
EntityFactory* EntityFactory::myInstance = nullptr;

EntityFactory* EntityFactory::GetInstance()
{
	if (myInstance == nullptr)
	{
		myInstance = new EntityFactory();
	}
	return myInstance;
}

void EntityFactory::Destroy()
{
	SAFE_DELETE(myInstance);
}

void EntityFactory::LoadEntities(const char* aEntityListXML)
{
	XMLReader entityListDocument;
	entityListDocument.OpenDocument(aEntityListXML);

	tinyxml2::XMLElement* rootElement = entityListDocument.ForceFindFirstChild("root");

	for (tinyxml2::XMLElement* e = entityListDocument.FindFirstChild(rootElement); e != nullptr;
		e = entityListDocument.FindNextElement(e))
	{
		std::string entityPath;
		entityListDocument.ForceReadAttribute(e, "src", entityPath);
		LoadEntity(entityPath.c_str());

		myFileWatcher.WatchFileChange(entityPath, std::bind(&EntityFactory::ReloadEntity, this, std::placeholders::_1));
	}

	entityListDocument.CloseDocument();
}


Entity* EntityFactory::CreateEntity(eEntityType aType, Prism::Scene* aScene, const CU::Vector3f& aPosition
	, const CU::Vector3f& aRotation, const CU::Vector3f& aScale, int aPlayerID)
{
	if (myInstance->myLoadedEntityData.find(aType) != myInstance->myLoadedEntityData.end())
	{

		Entity* newEntity = new Entity(myInstance->myLoadedEntityData.find(aType)->second, aScene, aPosition, aRotation
			, aScale, "", aPlayerID);
		return newEntity;
	}
	DL_ASSERT("Entity not found with enum: " + std::to_string(aType));
	return nullptr;
}


Entity* EntityFactory::CreateEntity(eEntityType, std::string aSubType, Prism::Scene* aScene, const CU::Vector3f& aPosition,
	const CU::Vector3f& aRotation, const CU::Vector3f& aScale, int aPlayerID)
{

		if (myInstance->myLoadedSubEntityData.find(aSubType) != myInstance->myLoadedSubEntityData.end())
		{

			Entity* newEntity = new Entity(myInstance->myLoadedSubEntityData.find(aSubType)->second, aScene, aPosition, aRotation
				, aScale, aSubType, aPlayerID);

				
			return newEntity;
		}
	std::string errorMessage = "SubType " + aSubType + " not found.";
	DL_ASSERT(errorMessage);
	return nullptr;
}

void EntityFactory::UpdateFileWatcher()
{
#ifndef RELEASE_BUILD
	myFileWatcher.FlushChanges();
#endif
}

void EntityFactory::LoadEntity(const char* aEntityPath)
{
	XMLReader entityDocument;
	entityDocument.OpenDocument(aEntityPath);

	tinyxml2::XMLElement* rootElement = entityDocument.ForceFindFirstChild("root");
	//tinyxml2::XMLElement* entityElement = entityDocument.ForceFindFirstChild(rootElement, "Entity");

	for (tinyxml2::XMLElement* entityElement = entityDocument.ForceFindFirstChild(rootElement, "Entity"); entityElement != nullptr;
		entityElement = entityDocument.FindNextElement(entityElement))
	{

		EntityData newData;
		std::string entityType;
		std::string entitySubType;
		newData.mySubType = "";
		entityDocument.ForceReadAttribute(entityElement, "type", entityType);
		newData.myType = EntityEnumConverter::ConvertStringToEntityType(CU::ToLower(entityType));

		entityDocument.ReadAttribute(entityElement, "subType", entitySubType);

		newData.mySubType = CU::ToLower(entitySubType);

		ReadComponents(entityDocument, entityElement, newData);
		

		if (newData.mySubType != "")
		{
			myLoadedSubEntityData[newData.mySubType] = newData;
		}
		else
		{
			myLoadedEntityData[newData.myType] = newData;
		}
	}
	entityDocument.CloseDocument();
}

void EntityFactory::ReloadEntity(const std::string& aEntityPath)
{
	SET_RUNTIME(false);

	XMLReader entityDocument;
	entityDocument.OpenDocument(aEntityPath);

	tinyxml2::XMLElement* rootElement = entityDocument.ForceFindFirstChild("root");
	//tinyxml2::XMLElement* entityElement = entityDocument.ForceFindFirstChild(rootElement, "Entity");

	for (tinyxml2::XMLElement* entityElement = entityDocument.ForceFindFirstChild(rootElement, "Entity"); entityElement != nullptr;
		entityElement = entityDocument.FindNextElement(entityElement))
	{
		std::string entityType;
		std::string entitySubType;
		entityDocument.ForceReadAttribute(entityElement, "type", entityType);
		entityDocument.ReadAttribute(entityElement, "subType", entitySubType);

		EntityData* data = nullptr;
		if (CU::ToLower(entitySubType) != "")
		{
			data = &myLoadedSubEntityData[CU::ToLower(entitySubType)];
		}
		else
		{
			data = &myLoadedEntityData[EntityEnumConverter::ConvertStringToEntityType(CU::ToLower(entityType))];
		}

		DL_ASSERT_EXP(data != nullptr, "Tried to reload entity that wasnt allready loaded");

		ReadComponents(entityDocument, entityElement, *data);
	}
	entityDocument.CloseDocument();

	RESET_RUNTIME;
}

void EntityFactory::ReadComponents(XMLReader& aReader, tinyxml2::XMLElement* aEntityElement, EntityData& someEntityData)
{
	for (tinyxml2::XMLElement* e = aReader.FindFirstChild(aEntityElement); e != nullptr;
		e = aReader.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());

		if (elementName == CU::ToLower("AnimationComponent"))
		{
			if (someEntityData.myGraphicsData.myExistsInEntity == true) DL_ASSERT("You have a GraphicsComponent so you can't have a AnimationComponent");
			if (someEntityData.myAnimationData.myExistsInEntity == true) DL_ASSERT("You already have a AnimationComponent");

			myComponentLoader->Load(aReader, e, someEntityData.myAnimationData);
		}
		else if (elementName == CU::ToLower("GraphicsComponent"))
		{
			if (someEntityData.myAnimationData.myExistsInEntity == true) DL_ASSERT("You have a AnimationComponent so you can't have a GraphicsComponent");
			if (someEntityData.myGraphicsData.myExistsInEntity == true) DL_ASSERT("You already have a GraphicsComponent");

			myComponentLoader->Load(aReader, e, someEntityData.myGraphicsData);
		}
		else if (elementName == CU::ToLower("InputComponent"))
		{
			myComponentLoader->Load(aReader, e, someEntityData.myInputData);
		}
		else if (elementName == CU::ToLower("MovementComponent"))
		{
			myComponentLoader->Load(aReader, e, someEntityData.myMovementData);
		}
		else if (elementName == CU::ToLower("TriggerComponent"))
		{
			myComponentLoader->Load(aReader, e, someEntityData.myTriggerData);
		}
		else if (elementName == CU::ToLower("PhysicsComponent"))
		{
			myComponentLoader->Load(aReader, e, someEntityData.myPhysicsData);
		}
		else if (elementName == CU::ToLower("SoundComponent"))
		{
			myComponentLoader->Load(aReader, e, someEntityData.mySoundData);
		}
		else if (elementName == CU::ToLower("SawBladeComponent"))
		{
			myComponentLoader->Load(aReader, e, someEntityData.mySawBladeData);
		}
		else if (elementName == CU::ToLower("PlayerGraphicsComponent"))
		{
			myComponentLoader->Load(aReader, e, someEntityData.myPlayerGraphicsData);
		}
		else if (elementName == CU::ToLower("SteamComponent"))
		{
			myComponentLoader->Load(aReader, e, someEntityData.mySteamData);
		}
		else if (elementName == CU::ToLower("StomperComponent"))
		{
			myComponentLoader->Load(aReader, e, someEntityData.myStomperData);
		}
		else if (elementName == CU::ToLower("AcidComponent"))
		{
			myComponentLoader->Load(aReader, e, someEntityData.myAcidData);
		}
		else if (elementName == CU::ToLower("ScoreComponent"))
		{
			myComponentLoader->Load(aReader, e, someEntityData.myScoreData);
		}
		else if (elementName == CU::ToLower("PlayerComponent"))
		{
			myComponentLoader->Load(aReader, e, someEntityData.myPlayerData);
		}
		else if (elementName == CU::ToLower("BounceComponent"))
		{
			myComponentLoader->Load(aReader, e, someEntityData.myBounceData);
		}
		else if (elementName == CU::ToLower("NoDecal"))
		{
			someEntityData.myShowDecal = false;
		}
		else
		{
			std::string errorMessage = "The component " + elementName +
				" is not Supported. Please check if you spelled correctly.";
			DL_ASSERT(errorMessage.c_str());
		}
	}
}

EntityFactory::EntityFactory()
{
	myComponentLoader = new ComponentLoader();
}


EntityFactory::~EntityFactory()
{
	SAFE_DELETE(myComponentLoader);
}