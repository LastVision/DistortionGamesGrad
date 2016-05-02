#include "stdafx.h"

#include "Level.h"
#include <Scene.h>

Level::Level(Prism::Camera& aCamera)
	: myCamera(aCamera)
	, myEntities(1024)
{
	myScene = new Prism::Scene();
}

Level::~Level()
{
	SAFE_DELETE(myScene);
	myEntities.DeleteAll();
}

void Level::Update(float aDelta)
{
	aDelta;
}

void Level::Render()
{

}