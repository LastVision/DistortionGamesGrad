
#include <CommonHelper.h>
#include "LevelReader.h"
#include <fstream>
#include <XMLReader.h>

LevelReader::LevelReader(const std::string& aFile)
	: myUsedIDs(64)
{
	myBuffer.open(aFile, std::ios::out);
	myOutputStream = new std::ostream(&myBuffer);

	*myOutputStream << "<root>";
}


LevelReader::~LevelReader()
{
	*myOutputStream << "\n</root>";
	myBuffer.close();
	delete myOutputStream;
	myOutputStream = nullptr;

	for (int i = 0; i < myUsedIDs.Size(); ++i)
	{
		if (myUsedIDs.Find(i + 1) == myUsedIDs.FoundNone)
		{
			DL_MESSAGE_BOX(CU::Concatenate("Level with ID %i not found", i + 1).c_str(), "ERROR", MB_ICONWARNING);
		}
	}
}

void LevelReader::ReadFile(const std::string& aFile)
{
	if (aFile.compare(aFile.size() - 4, 4, ".xml") == 0
		|| aFile.compare(aFile.size() - 4, 4, ".XML") == 0)
	{
		int slashIndex = aFile.rfind("/");
		std::string levelIDstr(aFile.begin() + slashIndex + 1, aFile.begin() + slashIndex + 4);

		int levelID = CU::StringToInt(levelIDstr);

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
