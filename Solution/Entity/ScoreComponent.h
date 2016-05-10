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