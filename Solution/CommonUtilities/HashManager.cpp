#include "stdafx.h"

#include "CommonHelper.h"
#include "MurmurHash3.h"
#include "HashManager.h"

namespace CU
{
	std::unordered_map<std::string, unsigned int> HashManager::myHashes;
	void HashManager::CheckFileHash(const std::string& aFilePath)
	{
		if (GC::HasCheatFiles == true)
		{
			return;
		}

		if (aFilePath.find("GeneratedData") != aFilePath.npos)
		{
			return;
		}

		if (myHashes.size() == 0)
		{
			CreateHashes();
		}

		if (myHashes.find(aFilePath) == myHashes.end())
		{
			GC::HasCheatFiles = true;
			return;
		}

		unsigned int newHash = Hash(CU::ReadFileToString(aFilePath));
		if (myHashes[aFilePath] != newHash)
		{
			GC::HasCheatFiles = true;
		}
	}

	void HashManager::CreateHashes()
	{
		DL_PRINT("CreateHashes()");
		std::fstream fileStream;
		fileStream.open("GeneratedData/Resource/Model/environment_blocks/saw_blades/blade_4x4_A.dgfx", std::ios::in | std::ios::binary);

		DL_ASSERT_EXP(fileStream.good() == true, "Failed to open Hash-File, did you forget to run the tool?");
		if (fileStream.good() == false)
		{
			return;
		}

		SET_RUNTIME(false);
		int hashCount = 0;
		fileStream.read((char*)&hashCount, sizeof(int));
		for (int i = 0; i < hashCount; ++i)
		{
			int fileLenght = 0;
			std::string file;
			unsigned int hash = 0;

			fileStream.read((char*)&fileLenght, sizeof(int)); //currentTexture.myFileName lenght
			char* texture = new char[fileLenght + 1];
			fileStream.read(texture, sizeof(char) * fileLenght); //currentTexture.myFileName
			texture[fileLenght] = '\0';
			file = texture;
			delete texture;

			fileStream.read((char*)&hash, sizeof(unsigned int)); //currentTexture.myFileName lenght

			myHashes[file] = hash;
		}
		RESET_RUNTIME;
		fileStream.close();
		DL_PRINT("CreateHashes - Finished()");
	}


}