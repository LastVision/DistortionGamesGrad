#include "stdafx.h"

#include "EntityEnumConverter.h"

namespace EntityEnumConverter
{
	const eEntityType ConvertStringToEntityType(const std::string& aType)
	{
		if (aType == "prop")
		{
			return eEntityType::PROP;
		}
		else if (aType == "spike")
		{
			return eEntityType::SPIKE;
		}
		else if (aType == "sawblade")
		{
			return eEntityType::SAW_BLADE;
		}
		else if (aType == "spike")
		{
			return eEntityType::SPIKE;
		}
		else if (aType == "steam")
		{
			return eEntityType::STEAM;
		}
		else if (aType == "steamvent")
		{
			return eEntityType::STEAM_VENT;
		}
		else if (aType == "bouncer")
		{
			return eEntityType::BOUNCER;
		}
		else if (aType == "player")
		{
			return eEntityType::PLAYER;
		}
		else if (aType == "spawnpoint")
		{
			return eEntityType::SPAWN_POINT;
		}
		else if (aType == "goalpoint")
		{
			return eEntityType::GOAL_POINT;
		}
		else if (aType == "scrap")
		{
			return eEntityType::SCRAP;
		}
		else if (aType == "stomper")
		{
			return eEntityType::STOMPER;
		}
		else if (aType == "stomper_holder")
		{
			return eEntityType::STOMPER_HOLDER;
		}
		else if (aType == "acid")
		{
			return eEntityType::ACID;
		}
		else if (aType == "acid_drop")
		{
			return eEntityType::ACID_DROP;
		}

		DL_ASSERT("The " + aType + " entity type is not supported, please tell Daniel about it.");
		return eEntityType::_COUNT;
	}
	const eOwnerType ConvertStringToOwnerType(const std::string& aType)
	{
		aType;

		DL_ASSERT("Not in use. Remove assert when in use.");
		return eOwnerType::NOT_USED;
	}

	const eWeaponType ConvertStringToWeaponType(const std::string& aType)
	{
		if (aType == "pistol")
		{
			return eWeaponType::PISTOL;
		}
		else if (aType == "shotgun")
		{
			return eWeaponType::SHOTGUN;
		}
		else if (aType == "grenade_launcher")
		{
			return eWeaponType::GRENADE_LAUNCHER;
		}

		DL_ASSERT("The " + aType + " weapon type is not supported.");
		return eWeaponType::PISTOL;
	}
}