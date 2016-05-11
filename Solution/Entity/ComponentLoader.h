#pragma once

struct AnimationComponentData;
struct BounceComponentData;
struct GraphicsComponentData;
struct InputComponentData;
struct MovementComponentData;
struct PhysicsComponentData;
struct TriggerComponentData;
struct SoundComponentData;
struct InputComponentData;
struct SawBladeComponentData;
struct PlayerGraphicsComponentData;
struct SteamComponentData;
struct ScoreComponentData;
struct PlayerComponentData;
class XMLReader;

namespace tinyxml2
{
	class XMLElement;
}

class ComponentLoader
{
public:
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, AnimationComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, BounceComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, GraphicsComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, InputComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, MovementComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, PhysicsComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, TriggerComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, SoundComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, SawBladeComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, PlayerGraphicsComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, SteamComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, ScoreComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, PlayerComponentData& aOutputData);

private:

	int ConvertToTriggerEnum(std::string aName);
};