#pragma once
#include "IReader.h"
#include <unordered_map>

class HashReader : public IReader
{
public:
	HashReader();
	~HashReader();

	virtual void ReadFile(const std::string& aFile) override;

private:
	std::unordered_map<std::string, unsigned int> myHashes;
};

