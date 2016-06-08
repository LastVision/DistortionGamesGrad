#include "stdafx.h"
#include "Subscriber.h"
#include "Message.h"

Subscriber::Subscriber(){}
Subscriber::~Subscriber(){}

void Subscriber::ReceiveMessage(const FadeMessage&){}
void Subscriber::ReceiveMessage(const EmitterMessage&){}
void Subscriber::ReceiveMessage(const GameStateMessage&){}
void Subscriber::ReceiveMessage(const Message&){}
void Subscriber::ReceiveMessage(const OnClickMessage&){}
void Subscriber::ReceiveMessage(const ResizeMessage&){}
void Subscriber::ReceiveMessage(const FinishLevelMessage&){}
void Subscriber::ReceiveMessage(const PlayerActiveMessage&){}
void Subscriber::ReceiveMessage(const ScrapMessage&){}
void Subscriber::ReceiveMessage(const OnDeathMessage&){}
void Subscriber::ReceiveMessage(const OnPlayerLevelComplete&){}
void Subscriber::ReceiveMessage(const OnPlayerJoin&){}
void Subscriber::ReceiveMessage(const ReturnToMenuMessage&){}
void Subscriber::ReceiveMessage(const ReachedGoalMessage&) {}
void Subscriber::ReceiveMessage(const UnhoverControllerMessage&) {}
void Subscriber::ReceiveMessage(const NightmareIsLockedMessage&) {}
void Subscriber::ReceiveMessage(const ReturnToMainMenuMessage&) {}
void Subscriber::ReceiveMessage(const SoundMessage&){}