#include "stdafx.h"
#include <InputWrapper.h>
#include "AnimationComponent.h"
#include "AnimationSystem.h"
#include <Effect.h>
#include <Engine.h>
#include <EngineEnums.h>
#include <EffectContainer.h>
#include <Instance.h>
#include <Model.h>
#include <ModelLoader.h>
#include <ModelProxy.h>
#include <Scene.h>
#include <Texture.h>

//#define BOX_MODE

AnimationComponent::AnimationComponent(Entity& aEntity, const AnimationComponentData& aComponentData, Prism::Scene*)
	: Component(aEntity)
	, myComponentData(aComponentData)
	, myInstance(nullptr)
	, myCullingRadius(50.f)
{
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModelAnimated(myComponentData.myModelPath
		, myComponentData.myEffectPath);

	myInstance = new Prism::Instance(*model, myEntity.GetOrientation());
}

AnimationComponent::~AnimationComponent()
{
	myEntity.GetScene()->RemoveInstance(myInstance);
	SAFE_DELETE(myInstance);
}

void AnimationComponent::Reset()
{
}

void AnimationComponent::Update(float aDeltaTime)
{
	myInstance->Update(aDeltaTime);
}