#pragma once
#include "IReader.h"

#include <fstream>
class EntityReader : public IReader
{
public:
	EntityReader();
	~EntityReader();

	void ReadFile(const std::string& aFile) override;

private:
	std::ostream* myOutputStream;
	std::filebuf myBuffer;
};

