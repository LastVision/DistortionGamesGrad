#pragma once

#include <CommonHelper.h>
#include <GrowingArray.h>
#include "Message.h"
#include "MessageEnum.h"
#include <StaticArray.h>
#include "Subscriber.h"
#include <unordered_map>
#include <Vector.h>
#include <VectorOnStack.h>

#undef SendMessage

class PostMaster
{
public:
	static PostMaster* GetInstance();
	static void Create();
	static void Destroy();

	void Subscribe(Subscriber* aSubscriber, int someMessageFlags);

	//Set someMessageFlags to 0 to unsubscribe to everything
	void UnSubscribe(Subscriber* aSubscriber, int someMessageFlags);

	bool IsSubscribed(Subscriber* aSubscriber, eMessageType aMessageType);

	template<typename Message>
	void SendMessage(const Message& aMessage);

private:
	PostMaster();
	~PostMaster();
	static PostMaster* myInstance;

	struct SubscriberInfo
	{
		SubscriberInfo(int aFlag = 0)
			: mySubscribers(64)
			, myFlag(aFlag)
		{}

		CU::GrowingArray<Subscriber*> mySubscribers;
		int myFlag;
	};

	CU::GrowingArray<SubscriberInfo> mySubscribers;
};

template<typename Message>
void PostMaster::SendMessage(const Message& aMessage)
{
	for each (const SubscriberInfo& info in mySubscribers)
	{
		if ((info.myFlag & aMessage.myMessageType) > 0)
		{
#ifdef _DEBUG
			if (info.mySubscribers.Size() == 0
				//&& aMessage.myMessageType != eMessageType::RESIZE
				)
			{
				DL_ASSERT(CU::Concatenate("Cant send message %i without subscriber", aMessage.myMessageType));
			}
#endif

			for each (Subscriber* subscriber in info.mySubscribers)
			{
				subscriber->ReceiveMessage(aMessage);
			}
		}
	}
}