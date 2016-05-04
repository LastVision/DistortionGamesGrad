#include "EntityReader.h"
#include <fstream>

EntityReader::EntityReader()
{
	myBuffer.open("GeneratedData/LI_entity.xml", std::ios::out);
	myOutputStream = new std::ostream(&myBuffer);

	*myOutputStream << "<root>";
}


EntityReader::~EntityReader()
{
	*myOutputStream << "\n</root>";
	myBuffer.close();
	delete myOutputStream;
	myOutputStream = nullptr;
}

void EntityReader::ReadFile(const std::string& aFile)
{
	if (aFile.compare(aFile.size() - 4, 4, ".xml") == 0
		|| aFile.compare(aFile.size() - 4, 4, ".XML") == 0)
	{
		*myOutputStream << "\n\t<path src=\"" << aFile << "\"/>";
	}
}
