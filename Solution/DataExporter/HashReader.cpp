#include <CommonHelper.h>
#include "HashReader.h"
#include <MurmurHash3.h>

#include <fstream>

HashReader::HashReader()
{
}


HashReader::~HashReader()
{
	std::fstream fileStream;
	fileStream.open("GeneratedData/Resource/Model/environment_blocks/rock_blocks/rocks_5x4_a.dgfx", std::ios::out | std::ios::binary);

	int hashCount = myHashes.size() + 1;
	fileStream.write((char*)&hashCount, sizeof(int));

	for (auto it = myHashes.begin(); it != myHashes.end(); ++it)
	{
		std::string file = it->first;
		int fileLenght = file.length();
		unsigned int hash = it->second;

		fileStream.write((char*)&fileLenght, sizeof(int));
		fileStream.write(file.c_str(), sizeof(char) * fileLenght);
		fileStream.write((char*)&hash, sizeof(unsigned int));


		//int textureLenght = currentTexture.myFileName.length();
		//aStream.write((char*)&textureLenght, sizeof(int)); //currentTexture.myFileName lenght
		//const char* texture = currentTexture.myFileName.c_str();
		//aStream.write(texture, sizeof(char) * textureLenght); //currentTexture.myFileName
	}

	fileStream.close();

	fileStream.open("GeneratedData/Resource/Model/environment_blocks/rock_blocks/rocks_5x4_a.xml", std::ios::out);
	fileStream << "<root>\n\t<radius value=\"5.35752\"/>\n</root>";
	fileStream.close();
}

void HashReader::ReadFile(const std::string& aFile)
{
	if (aFile.compare(aFile.size() - 4, 4, ".xml") == 0
		|| aFile.compare(aFile.size() - 4, 4, ".XML") == 0)
	{
		unsigned int hash = Hash(CU::ReadFileToString(aFile));
		myHashes[aFile] = hash;
	}
}
