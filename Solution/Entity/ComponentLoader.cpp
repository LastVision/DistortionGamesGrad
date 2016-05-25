#include "stdafx.h"

#include "BounceComponentData.h"
#include <CommonHelper.h>
#include "ComponentLoader.h"
#include "EntityEnumConverter.h"
#include "PhysicsComponentData.h"
#include "TriggerComponentData.h"
#include "ScoreComponentData.h"
#include "SawBladeComponentData.h"
#include "SteamComponentData.h"
#include "StomperComponentData.h"
#include "PlayerComponentData.h"
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
	}
}

void ComponentLoader::Load(XMLReader&, tinyxml2::XMLElement*, BounceComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
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

void ComponentLoader::Load(XMLReader&, tinyxml2::XMLElement*, InputComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, MovementComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "Gravity"), "value", aOutputData.myGravity);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "Impulse"), "value", aOutputData.myImpulse);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "RotationSpeed"), "value", aOutputData.myRotationSpeed);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "Drag"), "x", aOutputData.myDrag.x);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "Drag"), "y", aOutputData.myDrag.y);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "DeadZone"), "value", aOutputData.myDeadZone);
	float angleEpsilonDegree;
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "AngleEpsilon"), "value", angleEpsilonDegree);
	aOutputData.myAngleEpsilon = CU::Math::DegreeToRad(angleEpsilonDegree);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "WalkSpeedWithLegs"), "value", aOutputData.myWalkSpeedWithLegs);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "WalkSpeedWithoutLegs"), "value", aOutputData.myWalkSpeedWithoutLegs);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "WalkDrag"), "value", aOutputData.myWalkDrag);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "DashAimTime"), "value", aOutputData.myDashAimTime);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "DashFlyTime"), "value", aOutputData.myDashFlyTime);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "DashDistance"), "value", aOutputData.myDashDistance);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "DashCooldown"), "value", aOutputData.myDashCooldown);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "MaxAngleWhenLanding"), "value", aOutputData.myMaxAngleWhenLanding);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "SteamMinForce"), "value", aOutputData.mySteamMinForce);

	aOutputData.myDashSpeed = aOutputData.myDashDistance / aOutputData.myDashFlyTime;
}

void ComponentLoader::Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, PhysicsComponentData& aOutputData)
{
	std::string physicsType;
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "type"), "value", physicsType);

	tinyxml2::XMLElement* sphereElement = aDocument.FindFirstChild(aSourceElement, "isSphere");
	tinyxml2::XMLElement* activeFromStartElement = aDocument.FindFirstChild(aSourceElement, "isActiveFromStart");
	
	if (sphereElement != nullptr)
	{
		aDocument.ForceReadAttribute(sphereElement, "value", aOutputData.myIsSphere);
	}

	if (activeFromStartElement != nullptr)
	{
		aDocument.ForceReadAttribute(activeFromStartElement, "value", aOutputData.myIsActiveFromStart);
	}

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

	tinyxml2::XMLElement* objPathElement = aDocument.FindFirstChild(aSourceElement, "objPath");
	if (objPathElement != nullptr)
	{
		aDocument.ForceReadAttribute(objPathElement, "path", aOutputData.myObjPath);
	}
}

void ComponentLoader::Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, TriggerComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Trigger"))
		{
			std::string name = "";

			aDocument.ForceReadAttribute(e, "type", name);

			aOutputData.myTriggerType = ConvertToTriggerEnum(name);
		}	
	}
}

void ComponentLoader::Load(XMLReader&, tinyxml2::XMLElement*, SoundComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::Load(XMLReader&, tinyxml2::XMLElement*, SawBladeComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, PlayerGraphicsComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "Shader"), "path", aOutputData.myShader);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "AnimationShader"), "path", aOutputData.myAnimationShader);

	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "PlayerOneArrow"), "path", aOutputData.myPlayerOneArrow);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "PlayerTwoArrow"), "path", aOutputData.myPlayerTwoArrow);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "Body"), "path", aOutputData.myBody);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "Head"), "path", aOutputData.myHead);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "LeftLeg"), "path", aOutputData.myLeftLeg);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "RightLeg"), "path", aOutputData.myRightLeg);

	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "Idleanimation"), "path", aOutputData.myIdleAnimation);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "Walkanimation"), "path", aOutputData.myWalkAnimation);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "Flyanimation"), "path", aOutputData.myFlyAnimation);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "Dashaimanimation"), "path", aOutputData.myDashAimAnimation);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "Dashflyanimation"), "path", aOutputData.myDashFlyAnimation);
}
void ComponentLoader::Load(XMLReader&, tinyxml2::XMLElement*, ScoreComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::Load(XMLReader&, tinyxml2::XMLElement*, SteamComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::Load(XMLReader&, tinyxml2::XMLElement*, StomperComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;
}

void ComponentLoader::Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, PlayerComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "DeathSpeed"), "value", aOutputData.myDeathSpeed);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "LoseHeadSpeed"), "value", aOutputData.myLoseHeadSpeed);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "HeadDistance"), "value", aOutputData.myHeadDistance);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "LoseLegsSpeed"), "value", aOutputData.myLoseLegsSpeed);
	aDocument.ForceReadAttribute(aDocument.ForceFindFirstChild(aSourceElement, "LegsDistance"), "value", aOutputData.myLegsDistance);
}

int ComponentLoader::ConvertToTriggerEnum(std::string aName)
{
	if (aName == "hazard")
	{
		return static_cast<int>(eTriggerType::HAZARD);
	}
	else if (aName == "force")
	{
		return static_cast<int>(eTriggerType::FORCE);
	}
	else if (aName == "finish")
	{
		return static_cast<int>(eTriggerType::FINISH);
	}

	DL_ASSERT("[ComponentLoader] No trigger type in trigger component named " + aName);
	return static_cast<int>(eTriggerType::_COUNT);
}

