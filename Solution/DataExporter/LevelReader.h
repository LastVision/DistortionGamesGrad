#pragma once
#include "IReader.h"
#include <fstream>
#include <GrowingArray.h>

class LevelReader : public IReader
{
public:
	LevelReader(const std::string& aFile, const std::string& aLevelCountPath);
	~LevelReader();

	void ReadFile(const std::string& aFile) override;

private:
	std::ostream* myOutputStream;
	std::filebuf myBuffer;

	CU::GrowingArray<int> myUsedIDs;
	std::string myLevelCountPath;

	int myLevelCount;
};

