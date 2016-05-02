#pragma once
#include <GrowingArray.h>
#include "Subscriber.h"

class Entity;

namespace Prism
{
	class Camera;
	class Scene;
};

class Level : public Subscriber
{
	friend class LevelFactory;
public:
	Level(Prism::Camera& aCamera);
	~Level();

	void Update(float aDelta);
	void Render();

private:
	void operator=(Level&) = delete;

	Prism::Camera& myCamera;
	Prism::Scene* myScene;

	CU::GrowingArray<Entity*> myEntities;
};

