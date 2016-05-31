#pragma once
#include <thread>

namespace CU
{
	void SetThreadName(const std::thread::id& anID, const char* aThreadName);
};