#include "stdafx.h"

#include "PostMaster.h"

PostMaster* PostMaster::myInstance = nullptr;
PostMaster::PostMaster()
	: mySubscribers(64)
{
	for (int i = 1; i < eMessageType::_POSTMASTER_COUNT; i *= 2)
	{
		mySubscribers.Add(SubscriberInfo(i));
	}
}


PostMaster::~PostMaster()
{
	for (int i = 0; i < mySubscribers.Size(); ++i)
	{
		if (mySubscribers[i].mySubscribers.Size() > 0)
		{
			DL_ASSERT(CU::Concatenate("Subscriber not unsubscribed at flag %i at Postmaster-Destroy", mySubscribers[i].myFlag));
		}

		mySubscribers[i].mySubscribers.RemoveAll();
	}
}

PostMaster* PostMaster::GetInstance()
{
	DL_ASSERT_EXP(myInstance != nullptr, "Need to create Postmaster before getting it.");
	return myInstance;
}

void PostMaster::Create()
{
	DL_ASSERT_EXP(myInstance == nullptr, "Postmaster already created.");

	myInstance = new PostMaster();
}

void PostMaster::Destroy()
{
	DL_ASSERT_EXP(myInstance != nullptr, "Need to create Postmaster before destroying it.");

	
	delete myInstance;
	myInstance = nullptr;
}

void PostMaster::Subscribe(Subscriber* aSubscriber, int someMessageFlags)
{
	bool success = false;
	
	for (SubscriberInfo& info : mySubscribers)
	{
		if ((info.myFlag & someMessageFlags) > 0)
		{
#ifdef _DEBUG
			for each (const Subscriber* subscriber in info.mySubscribers)
			{
				DL_ASSERT_EXP(subscriber != aSubscriber, "Tried to add the same subscriber to the same message twice.");
			}
#endif

			success = true;
			info.mySubscribers.Add(aSubscriber);
		}
	}

	DL_ASSERT_EXP(success == true, CU::Concatenate("Cant subscribe with flags %i", someMessageFlags));
}

void PostMaster::UnSubscribe(Subscriber* aSubscriber, int someMessageFlags)
{
	bool success = false;

	for (SubscriberInfo& info : mySubscribers)
	{
		if (someMessageFlags == 0)
		{
			if (IsSubscribed(aSubscriber, static_cast<eMessageType>(info.myFlag)))
			{
				success = true;
				info.mySubscribers.RemoveCyclic(aSubscriber);
			}
		}
		else if ((info.myFlag & someMessageFlags) > 0)
		{
			DL_ASSERT_EXP(info.mySubscribers.Find(aSubscriber) != info.mySubscribers.FoundNone, CU::Concatenate("Cant unsubscribe with flag %i", info.myFlag));

			success = true;
			info.mySubscribers.RemoveCyclic(aSubscriber);
		}
	}

	DL_ASSERT_EXP(success == true, CU::Concatenate("Cant unsubscribe with flag %i", someMessageFlags));
}

bool PostMaster::IsSubscribed(Subscriber* aSubscriber, eMessageType aMessageType)
{
	for each (const SubscriberInfo& info in mySubscribers)
	{
		if ((info.myFlag & aMessageType) > 0)
		{
			return info.mySubscribers.Find(aSubscriber) != info.mySubscribers.FoundNone;
		}
	}

	return false;
}