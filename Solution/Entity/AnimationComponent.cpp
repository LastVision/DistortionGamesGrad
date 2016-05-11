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
	, myCullingRadius(10.f)
	, myIsEnemy(false)
{
#ifndef BOX_MODE
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModelAnimated(myComponentData.myModelPath
		, myComponentData.myEffectPath);

	myInstance = new Prism::Instance(*model, myEntity.myOrientation);

	int animations = static_cast<int>(eEntityState::_COUNT);

	for (int i = 0; i < animations; ++i)
	{
		AnimationLoadData loadAnimation = myComponentData.myAnimations[i];
		AddAnimation(loadAnimation.myEntityState, loadAnimation.myAnimationPath, loadAnimation.myLoopFlag, loadAnimation.myResetTimeOnRestart);
	}

#endif
}

AnimationComponent::~AnimationComponent()
{
	myEntity.GetScene()->RemoveInstance(myInstance);
	SAFE_DELETE(myInstance);
	SAFE_DELETE(myWeapon);
}

void AnimationComponent::Reset()
{
	if (myIsEnemy == true)
	{
		myWeapon->SetShouldRender(true);
	}
	myPrevEntityState = myEntity.GetState();
	PlayAnimation(myEntity.GetState());
}

void AnimationComponent::AddAnimation(eEntityState aState, const std::string& aAnimationPath
	, bool aLoopFlag, bool aResetTimeOnRestart)
{
	Prism::AnimationSystem::GetInstance()->GetAnimation(aAnimationPath.c_str());
	AnimationData newData;
	newData.myElapsedTime = 0.f;
	newData.myFile = aAnimationPath;
	newData.myShouldLoop = aLoopFlag;
	newData.myResetTimeOnRestart = aResetTimeOnRestart;
	myAnimations[int(aState)] = newData;
}

void AnimationComponent::Update(float aDeltaTime)
{
	if (myIsEnemy == true && myPrevEntityState != eEntityState::DIE && myEntity.GetState() == eEntityState::DIE)
	{
		myWeapon->SetShouldRender(false);
	}

	AnimationData& data = myAnimations[int(myEntity.GetState())];
	if (myPrevEntityState != myEntity.GetState())
	{
		PlayAnimation(myEntity.GetState());
		myPrevEntityState = myEntity.GetState();
	}

	if (myInstance->IsAnimationDone() == false || data.myShouldLoop == true)
	{
		myInstance->Update(aDeltaTime);
	}

	data.myElapsedTime += aDeltaTime;

	if (myIsEnemy == true)
	{
		//myWeapon->SetAnimation(Prism::AnimationSystem::GetInstance()->GetAnimation(data.myFile.c_str()));
		myWeapon->Update(aDeltaTime);
		EnemyAnimationBone currentAnimation = myEnemyAnimations[int(myEntity.GetState())];
		if (currentAnimation.IsValid())
		{
			myWeaponJoint = CU::InverseSimple(*currentAnimation.myWeaponBone.myBind) * (*currentAnimation.myWeaponBone.myJoint) * myEntity.GetOrientation();
		}
	}
	else
	{
	}

	if (myEntity.GetState() == eEntityState::ATTACK && IsCurrentAnimationDone() == true)
	{
		myEntity.SetState(eEntityState::IDLE);
	}
}

bool AnimationComponent::IsCurrentAnimationDone() const
{
	return myInstance->IsAnimationDone();
}

void AnimationComponent::RestartCurrentAnimation()
{
	myInstance->ResetAnimationTime(0.f);
}

void AnimationComponent::PlayAnimation(eEntityState aAnimationState)
{
	AnimationData& data = myAnimations[int(aAnimationState)];
	myInstance->SetAnimation(Prism::AnimationSystem::GetInstance()->GetAnimation(data.myFile.c_str()));

	if (data.myResetTimeOnRestart == true)
	{
		myInstance->ResetAnimationTime(0.f);
	}
	else
	{
		myInstance->ResetAnimationTime(data.myElapsedTime);
	}
}

void AnimationComponent::SetRotation(const CU::Vector3<float>& aRotation)
{
	CU::Vector3f position(myEntity.myOrientation.GetPos());
	myEntity.myOrientation.SetPos(CU::Vector3f());

	myEntity.myOrientation = CU::Matrix44f::CreateRotateAroundX(aRotation.x) * myEntity.myOrientation;
	myEntity.myOrientation = CU::Matrix44f::CreateRotateAroundY(aRotation.y) * myEntity.myOrientation;
	myEntity.myOrientation = CU::Matrix44f::CreateRotateAroundZ(aRotation.z) * myEntity.myOrientation;

	myEntity.myOrientation.SetPos(position);
}

void AnimationComponent::SetScale(const CU::Vector3<float>& aScale)
{
	myInstance->SetScale(aScale);
}

Prism::Animation* AnimationComponent::GetCurrent() const
{
	return myInstance->GetCurrentAnimation();
}

void AnimationComponent::AddWeaponToScene(Prism::Scene* aScene)
{
	if (myIsEnemy == true)
	{
		aScene->AddInstance(myWeapon);
	}
}

void AnimationComponent::RemoveWeaponFromScene(Prism::Scene* aScene)
{
	if (myIsEnemy == true)
	{
		aScene->RemoveInstance(myWeapon);
	}
}