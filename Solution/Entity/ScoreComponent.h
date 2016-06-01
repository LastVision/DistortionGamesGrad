#pragma once
#include "Component.h"
#include "Score.h"

class ScoreComponent : public Component
{
public:
	ScoreComponent(Entity& anEntity);
	~ScoreComponent();

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void ReceiveNote(const DeathNote& aMessage) override;
	void ReceiveNote(const SpawnNote& aMessage) override;
	void ReceiveNote(const ReachedGoalNote& aMessage) override;

	const Score* GetScore() const;

	void ReachedGoal();

private:
	Score myScore;
};

inline eComponentType ScoreComponent::GetTypeStatic()
{
	return eComponentType::SCORE;
}

inline eComponentType ScoreComponent::GetType()
{
	return GetTypeStatic();
}

inline const Score* ScoreComponent::GetScore() const
{
	return &myScore;
}