#include "stdafx.h"

#include "AnimationComponent.h"
#include "AnimationSystem.h"
#include <AudioInterface.h>
#include <Effect.h>
#include <Engine.h>
#include <EngineEnums.h>
#include <EffectContainer.h>
#include <InputWrapper.h>
#include <Instance.h>
#include <Model.h>
#include <ModelLoader.h>
#include <ModelProxy.h>
#include <PostMaster.h>
#include <Scene.h>
#include "SoundComponent.h"
#include <Texture.h>


AnimationComponent::AnimationComponent(Entity& aEntity, const AnimationComponentData& aComponentData, Prism::Scene*)
	: Component(aEntity)
	, myComponentData(aComponentData)
	, myInstance(nullptr)
	, myCullingRadius(50.f)
{
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModelAnimated(myComponentData.myModelPath
		, myComponentData.myEffectPath);

	myInstance = new Prism::Instance(*model, myEntity.GetOrientation());

	PostMaster::GetInstance()->Subscribe(this, eMessageType::PLAYER_ACTIVE);
}

AnimationComponent::~AnimationComponent()
{
	PostMaster::GetInstance()->Subscribe(this, 0);
	SAFE_DELETE(myInstance);
}

void AnimationComponent::Reset()
{
	myInstance->ResetAnimationTime(0.f);
}

void AnimationComponent::Update(float aDeltaTime)
{
	if (myInstance->IsAnimationDone() == false || myInstance->AnimationIsLoaded() == false)
	{
		myInstance->Update(aDeltaTime);
	}
}

void AnimationComponent::ReceiveNote(const BounceNote&)
{
	myInstance->ResetAnimationTime(0.f);
	SoundComponent* soundComp = myEntity.GetComponent<SoundComponent>();
	if (soundComp != nullptr)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Bouncer", soundComp->GetAudioSFXID());
	}
}

void AnimationComponent::ReceiveMessage(const PlayerActiveMessage&)
{
	if (myEntity.GetType() == eEntityType::SPAWN_POINT)
	{
		myInstance->ResetAnimationTime(0.f);
	}
}
