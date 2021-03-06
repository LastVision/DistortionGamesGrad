#pragma once

struct FadeMessage;
struct EmitterMessage;
struct GameStateMessage;
struct HighscoreSetLevelMessage;
struct OnClickMessage;
struct ResizeMessage;
struct Message;
struct FinishLevelMessage;
struct PlayerActiveMessage;
struct OnPlayerLevelComplete;
struct OnDeathMessage;
struct ScrapMessage;
struct OnPlayerJoin;
struct ReturnToMenuMessage;
struct ReachedGoalMessage;
struct UnhoverControllerMessage;
struct NightmareIsLockedMessage;
struct ReturnToMainMenuMessage;
struct ScoreIsLoadingMessage;
struct SoundMessage;

class Subscriber
{
public:
	Subscriber();
	~Subscriber();

	virtual void ReceiveMessage(const FadeMessage& aMessage);
	virtual void ReceiveMessage(const EmitterMessage& aMessage);
	virtual void ReceiveMessage(const GameStateMessage& aMessage);
	virtual void ReceiveMessage(const HighscoreSetLevelMessage& aMessage);
	virtual void ReceiveMessage(const Message& aMessage);
	virtual void ReceiveMessage(const OnClickMessage& aMessage);
	virtual void ReceiveMessage(const ResizeMessage& aMessage);
	virtual void ReceiveMessage(const FinishLevelMessage& aMessage);
	virtual void ReceiveMessage(const PlayerActiveMessage& aMessage);
	virtual void ReceiveMessage(const ScrapMessage& aMessage);
	virtual void ReceiveMessage(const OnDeathMessage& aMessage);
	virtual void ReceiveMessage(const OnPlayerLevelComplete& aMessage);
	virtual void ReceiveMessage(const OnPlayerJoin& aMessage);
	virtual void ReceiveMessage(const ReturnToMenuMessage& aMessage);
	virtual void ReceiveMessage(const ReachedGoalMessage& aMessage);
	virtual void ReceiveMessage(const UnhoverControllerMessage& aMessage);
	virtual void ReceiveMessage(const NightmareIsLockedMessage& aMessage);
	virtual void ReceiveMessage(const ReturnToMainMenuMessage& aMessage);
	virtual void ReceiveMessage(const ScoreIsLoadingMessage& aMessage);
	virtual void ReceiveMessage(const SoundMessage& aMessage);
};

