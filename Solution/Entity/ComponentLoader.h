#pragma once

struct AnimationComponentData;
struct GraphicsComponentData;
struct PhysicsComponentData;
struct TriggerComponentData;
struct SoundComponentData;
struct InputComponentData;
struct SawBladeComponentData;
struct SpikeComponentData;
class XMLReader;

namespace tinyxml2
{
	class XMLElement;
}

class ComponentLoader
{
public:
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, AnimationComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, GraphicsComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, PhysicsComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, TriggerComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, SoundComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, InputComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, SawBladeComponentData& aOutputData);
	void Load(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, SpikeComponentData& aOutputData);

private:

	int ConvertToTriggerEnum(std::string aName);
};