#include "stdafx.h"

#include <CommonHelper.h>
#include "ComponentLoader.h"
#include "EntityEnumConverter.h"
#include "PhysicsComponentData.h"
#include "TriggerComponentData.h"
#include "SpikeComponentData.h"
#include "SawBladeComponentData.h"
#include "XMLReader.h"
#include "GameEnum.h"

void ComponentLoader::Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, AnimationComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Model"))
		{
			aDocument.ForceReadAttribute(e, "modelPath", aOutputData.myModelPath);
			aDocument.ForceReadAttribute(e, "shaderPath", aOutputData.myEffectPath);
		}
		else if (elementName == CU::ToLower("Animation"))
		{
			AnimationLoadData newAnimation;
			int animState;

			aDocument.ForceReadAttribute(e, "path", newAnimation.myAnimationPath);
			aDocument.ForceReadAttribute(e, "state", animState);
			aDocument.ForceReadAttribute(e, "loop", newAnimation.myLoopFlag);
			aDocument.ForceReadAttribute(e, "resetTime", newAnimation.myResetTimeOnRestart);
			newAnimation.myEntityState = static_cast<eEntityState>(animState);

			aOutputData.myAnimations.Insert(static_cast<int>(newAnimation.myEntityState), newAnimation);
		}
	}
}

void ComponentLoader::Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, GraphicsComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Model"))
		{
			aDocument.ForceReadAttribute(e, "modelPath", aOutputData.myModelPath);
			aDocument.ForceReadAttribute(e, "shaderPath", aOutputData.myEffectPath);
		}
		else if (elementName == CU::ToLower("AlwaysRender"))
		{
			DL_ASSERT("Legacy XML");
		}
#ifdef RELEASE_BUILD
		else if (elementName == CU::ToLower("Debug"))
		{
			aOutputData.myExistsInEntity = false;
		}
#endif
	}
}

void ComponentLoader::Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, InputComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, MovementComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, PhysicsComponentData& aOutputData)
{
	std::string physicsType;
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "type"), "value", physicsType);

	aOutputData.myExistsInEntity = true;

	if (CU::ToLower(physicsType) == "static")
	{
		aOutputData.myPhysicsType = ePhysics::STATIC;
	}
	else if (CU::ToLower(physicsType) == "dynamic")
	{
		aOutputData.myPhysicsType = ePhysics::DYNAMIC;

		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "x", aOutputData.myPhysicsMin.x);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "y", aOutputData.myPhysicsMin.y);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "z", aOutputData.myPhysicsMin.z);

		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "x", aOutputData.myPhysicsMax.x);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "y", aOutputData.myPhysicsMax.y);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "z", aOutputData.myPhysicsMax.z);
	}
	else if (CU::ToLower(physicsType) == "phantom" || CU::ToLower(physicsType) == "trigger")
	{
		aOutputData.myPhysicsType = ePhysics::PHANTOM;

		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "x", aOutputData.myPhysicsMin.x);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "y", aOutputData.myPhysicsMin.y);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "z", aOutputData.myPhysicsMin.z);

		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "x", aOutputData.myPhysicsMax.x);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "y", aOutputData.myPhysicsMax.y);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "z", aOutputData.myPhysicsMax.z);
	}
	else if (CU::ToLower(physicsType) == "capsule")
	{
		aOutputData.myPhysicsType = ePhysics::CAPSULE;
	}
	else if (CU::ToLower(physicsType) == "kinematic")
	{
		aOutputData.myPhysicsType = ePhysics::KINEMATIC;

		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "x", aOutputData.myPhysicsMin.x);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "y", aOutputData.myPhysicsMin.y);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "min"), "z", aOutputData.myPhysicsMin.z);

		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "x", aOutputData.myPhysicsMax.x);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "y", aOutputData.myPhysicsMax.y);
		aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "max"), "z", aOutputData.myPhysicsMax.z);
	}
	else
	{
		//DL_ASSERT(CU::Concatenate("Invalid phyics-type on %s %s", entityType.c_str(), entitySubType.c_str()));
		DL_ASSERT("Failed to load PhysicsComponent");
	}
}

void ComponentLoader::Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, TriggerComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
	aOutputData.myIsOneTime = false; 
	aOutputData.myIsPressable = false;
	aOutputData.myTriggerType = -1;
	aOutputData.myPickupText = "";
	aOutputData.myPickupTextTime = 0.f;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Trigger"))
		{
			std::string name = "";

			aDocument.ForceReadAttribute(e, "type", name);
			aDocument.ReadAttribute(e, "value", aOutputData.myValue);
			aDocument.ReadAttribute(e, "oneTimeTrigger", aOutputData.myIsOneTime);
			aDocument.ForceReadAttribute(e, "isClientSide", aOutputData.myIsClientSide);
			aDocument.ReadAttribute(e, "activeFromStart", aOutputData.myIsActiveFromStart);
			aDocument.ReadAttribute(e, "isPressable", aOutputData.myIsPressable);
			aDocument.ReadAttribute(e, "pickupText", aOutputData.myPickupText);
			aDocument.ReadAttribute(e, "pickupTextTime", aOutputData.myPickupTextTime);

			aOutputData.myTriggerType = ConvertToTriggerEnum(name);

		}
		else if (elementName == CU::ToLower("Marker"))
		{
				aDocument.ReadAttribute(e, "positionx", aOutputData.myPosition.x);
				aDocument.ReadAttribute(e, "positiony", aOutputData.myPosition.y);
				aDocument.ReadAttribute(e, "positionz", aOutputData.myPosition.z);
				aDocument.ForceReadAttribute(e, "show", aOutputData.myShowMarker);
		}		
	}
}

void ComponentLoader::Load(XMLReader&, tinyxml2::XMLElement*, SoundComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, SawBladeComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
	}

void ComponentLoader::Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, SpikeComponentData& aOutputData)
	{
	aOutputData.myExistsInEntity = true;
	}

int ComponentLoader::ConvertToTriggerEnum(std::string aName)
	{
	if (aName == "spike")
	{
		return static_cast<int>(eTriggerType::SPIKE);
	}
	else if (aName == "saw_blade")
	{
		return static_cast<int>(eTriggerType::SAW_BLADE);
	}

	DL_ASSERT("[ComponentLoader] No trigger type in trigger component named " + aName);
	return static_cast<int>(eTriggerType::_COUNT);
}

