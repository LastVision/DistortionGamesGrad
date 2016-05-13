#include "stdafx.h"

#include "PollingStation.h"

PollingStation* PollingStation::myInstance = nullptr;
PollingStation* PollingStation::GetInstance()
{
	DL_ASSERT_EXP(myInstance != nullptr, "POLLINGSTATION NOT CREATED YET, RUN CREATE");
	return myInstance;
}

void PollingStation::Create()
{
	DL_ASSERT_EXP(myInstance == nullptr, "POLLINGSTATION ALREADY CREATED");
	if (myInstance == nullptr)
	{
		myInstance = new PollingStation();
	}
}

void PollingStation::Destroy()
{
	SAFE_DELETE(myInstance);
}

PollingStation::PollingStation()
	: myPlayersAlive(0)
{
}

PollingStation::~PollingStation()
{
}
