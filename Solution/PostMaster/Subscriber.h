#pragma once

struct FadeMessage;
struct EmitterMessage;
struct GameStateMessage;
struct OnClickMessage;
struct ResizeMessage;
struct Message;
struct FinishLevelMessage;
struct PlayerActiveMessage;
struct OnDeathMessage;
struct ScrapMessage;

class Subscriber
{
public:
	Subscriber();
	~Subscriber();

	virtual void ReceiveMessage(const FadeMessage& aMessage);
	virtual void ReceiveMessage(const EmitterMessage& aMessage);
	virtual void ReceiveMessage(const GameStateMessage& aMessage);
	virtual void ReceiveMessage(const Message& aMessage);
	virtual void ReceiveMessage(const OnClickMessage& aMessage);
	virtual void ReceiveMessage(const ResizeMessage& aMessage);
	virtual void ReceiveMessage(const FinishLevelMessage& aMessage);
	virtual void ReceiveMessage(const PlayerActiveMessage& aMessage);
	virtual void ReceiveMessage(const ScrapMessage& aMessage);
	virtual void ReceiveMessage(const OnDeathMessage& aMessage);

};

