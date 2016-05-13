#pragma once

class Entity;
class PollingStation
{
public:
	static PollingStation* GetInstance();
	static void Create();
	static void Destroy();

	void SetPlayersAlive(int aPlayersAlive);
	const int GetPlayersAlive() const;
	
private:
	PollingStation();
	~PollingStation();

	static PollingStation* myInstance;
	int myPlayersAlive;
};

inline void PollingStation::SetPlayersAlive(int aPlayersAlive)
{
	myPlayersAlive = aPlayersAlive;
}

inline const int PollingStation::GetPlayersAlive() const
{
	return myPlayersAlive;
}