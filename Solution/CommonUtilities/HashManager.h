#pragma once

#include <unordered_map>
#include <string>

namespace CU
{
	class HashManager
	{
	public:
		static void CheckFileHash(const std::string& aFilePath);

	private:
		static void CreateHashes();

		static std::unordered_map<std::string, unsigned int> myHashes;
	};
}