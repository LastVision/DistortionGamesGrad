#include "stdafx.h"

#include <ModelLoader.h>
#include <Instance.h>
#include "PlayerGraphicsComponent.h"


PlayerGraphicsComponent::PlayerGraphicsComponent(Entity& aEntity, const PlayerGraphicsComponentData& aData
		, const CU::Matrix44<float>& aEntityOrientation)
	: Component(aEntity)
	, myData(aData)
	, myEntityOrientation(aEntityOrientation)
{
}


PlayerGraphicsComponent::~PlayerGraphicsComponent()
{
}

void PlayerGraphicsComponent::Init()
{
	myBody = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel(myData.myBody, myData.myShader), myBodyMatrix);
	myBindPose = new Prism::Instance(
		*Prism::ModelLoader::GetInstance()->LoadModelAnimated(myData.myIdleAnimation, myData.myShader)
		, myEntityOrientation);


	while (Prism::ModelLoader::GetInstance()->IsLoading())
		;

}

void PlayerGraphicsComponent::Update(float aDeltaTime)
{

}
