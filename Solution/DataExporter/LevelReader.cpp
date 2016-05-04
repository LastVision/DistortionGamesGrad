
#include <CommonHelper.h>
#include "LevelReader.h"
#include <fstream>
#include <XMLReader.h>

LevelReader::LevelReader()
	: myUsedIDs(64)
{
	myBuffer.open("GeneratedData/LI_level.xml", std::ios::out);
	myOutputStream = new std::ostream(&myBuffer);

	*myOutputStream << "<root>";
}


LevelReader::~LevelReader()
{
	*myOutputStream << "\n</root>";
	myBuffer.close();
	delete myOutputStream;
	myOutputStream = nullptr;
}

void LevelReader::ReadFile(const std::string& aFile)
{
	if (aFile.compare(aFile.size() - 4, 4, ".xml") == 0
		|| aFile.compare(aFile.size() - 4, 4, ".XML") == 0)
	{

		XMLReader reader;
		reader.OpenDocument(aFile);

		tinyxml2::XMLElement* element = reader.ForceFindFirstChild("root");
		element = reader.ForceFindFirstChild(element, "scene");
		element = reader.ForceFindFirstChild(element, "levelData");
		element = reader.ForceFindFirstChild(element, "id");

		int levelID = -1;
		reader.ForceReadAttribute(element, "value", levelID);
		reader.CloseDocument();

		if (myUsedIDs.Find(levelID) != myUsedIDs.FoundNone)
		{
			DL_MESSAGE_BOX(CU::Concatenate("LevelID %i (%s) was found more than one time", levelID, aFile.c_str()).c_str(), "ERROR", MB_ICONWARNING);
		}
		else
		{
			myUsedIDs.Add(levelID);

			*myOutputStream << "\n\t<level ID=\"" << levelID << "\" path=\"" << aFile << "\"/>";
		}
	}
}
