#include "stdafx.h"

#include "Level.h"
#include <Scene.h>
#include <EntityFactory.h>
Level::Level(Prism::Camera& aCamera)
	: myCamera(aCamera)
	, myEntities(1024)
{
	myScene = new Prism::Scene();
	myScene->SetCamera(aCamera);

	EntityFactory::GetInstance()->LoadEntities("Data/Resource/Entity/LI_entity.xml");
	myPlayer = EntityFactory::CreateEntity(eEntityType::PLAYER, "player", myScene, CU::Vector3<float>());

	myPlayer->AddToScene();


}

Level::~Level()
{
	SAFE_DELETE(myScene);
	SAFE_DELETE(myPlayer);
	myEntities.DeleteAll();
}

void Level::Update(float aDelta)
{
	myPlayer->Update(aDelta);
}

void Level::Render()
{
	myScene->Render();
}