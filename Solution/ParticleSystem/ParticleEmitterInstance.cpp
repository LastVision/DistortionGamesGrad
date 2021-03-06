#include "stdafx.h"

#include "Camera.h"
#include <CommonHelper.h>
#include <d3dx11effect.h>
#include <MathHelper.h>
#include "ParticleEmitterInstance.h"
#include "VertexBufferWrapper.h"
#include <Effect.h>
#include <TextureContainer.h>
#include <Engine.h>
#include <Texture.h>
#include <Entity.h>
#include <InputComponent.h>
#include <PlayerGraphicsComponent.h>
#include <Hat.h>
#include <HatManager.h>

namespace Prism
{

	ParticleEmitterInstance::ParticleEmitterInstance()
		: myVertexWrapper(nullptr)
		, myEmissionTime(-1.f)
		, myParticleIndex(0)
		, myLiveParticleCount(0)
		, myOverrideDirection(false)
		, myParticleToGraphicsCard(256)
	{

	}

	ParticleEmitterInstance::~ParticleEmitterInstance()
	{
		if (myVertexWrapper != nullptr && myVertexWrapper->myVertexBuffer != nullptr)
		{
			myVertexWrapper->myVertexBuffer->Release();
		}

		SAFE_DELETE(myVertexWrapper);
	}

	void ParticleEmitterInstance::Initiate(ParticleEmitterData* aData, bool anAllowManyParticles)
	{
		myStates.reset();
		myParticleEmitterData = aData;
		myEmitterPath = myParticleEmitterData->myFilePath;

		int particleCount = static_cast<int>(myParticleEmitterData->myParticlesPerEmitt * myParticleEmitterData->myData.myParticleLifeTime / myParticleEmitterData->myEmissionRate) + 1;


		DL_PRINT(("Loading :" + myEmitterPath).c_str());
		DL_ASSERT_EXP(anAllowManyParticles == true || particleCount <= 201, "Can't have more than 201 particles in an emitter!");

		myGraphicalParticles.Init(particleCount);
		myLogicalParticles.Init(particleCount);

		for (int i = 0; i < particleCount; ++i)
		{
			GraphicalParticle tempGraphics;
			myGraphicalParticles.Add(tempGraphics);
			LogicalParticle tempLogic;
			myLogicalParticles.Add(tempLogic);
		}


		Reset();

		myTexture = myParticleEmitterData->myTexture;
		myEmissiveTexture = myParticleEmitterData->myEmissiveTexture;
		CreateVertexBuffer();
	}

	void ParticleEmitterInstance::ReleaseData()
	{
		myParticleEmitterData = nullptr;
	}

	void ParticleEmitterInstance::Render(Prism::Texture* aSceneTexture)
	{
		int toGraphicsCard = UpdateVertexBuffer();
		myParticleEmitterData->myEffect->SetTexture(myTexture);
		myParticleEmitterData->myEffect->SetEmissiveTexture(myEmissiveTexture);

		if (myParticleEmitterData->myHasHeatHaze == true)
		{
			if (myOffsetTexture != nullptr)
			{
				myOffsetTexture->SetResource(myOffset ? myOffset->GetShaderView() : nullptr);
			}

			if (mySceneTexture != nullptr)
			{
				mySceneTexture->SetResource(aSceneTexture ? aSceneTexture->GetShaderView() : nullptr);
			}
		}

		ID3D11DeviceContext* context = Engine::GetInstance()->GetContex();
		context->IASetVertexBuffers(
			myVertexWrapper->myStartSlot
			, myVertexWrapper->myNumberOfBuffers
			, &myVertexWrapper->myVertexBuffer
			, &myVertexWrapper->myStride
			, &myVertexWrapper->myByteOffset);


		ID3DX11EffectTechnique* tech = myParticleEmitterData->myEffect->GetTechnique();
		if (myParticleEmitterData->myHasHeatHaze == true)
		{
			tech = myParticleEmitterData->myEffect->GetTechnique("Render_HeatHaze");
		}
		tech->GetDesc(myParticleEmitterData->myTechniqueDesc);

		for (UINT i = 0; i < myParticleEmitterData->myTechniqueDesc->Passes; ++i)
		{
			tech->GetPassByIndex(i)->Apply(0, context);
			context->Draw(toGraphicsCard, 0);
		}
		myParticleEmitterData->myEffect->SetTexture(NULL);
	}

	void ParticleEmitterInstance::Update(float aDeltaTime)
	{
		UpdateEmitter(aDeltaTime);
	}

	void ParticleEmitterInstance::CheckFlags()
	{
		if (myParticleEmitterData->myIsActiveAtStart == true)
		{
			myStates[ACTIVE] = TRUE;
		}

		if (myParticleEmitterData->myIsHollow == true)
		{
			myStates[HOLLOW] = TRUE;
		}

		if (myParticleEmitterData->myIsCircle == true)
		{
			myStates[CIRCLE] = TRUE;
		}

		if (myParticleEmitterData->myUseEmitterLifeTime == true)
		{
			myStates[EMITTERLIFE] = TRUE;
		}

		if (myParticleEmitterData->myIsSphere == true)
		{
			myStates[SPHERE] = TRUE;
		}

		if (myParticleEmitterData->myUseAlphaDelta == true)
		{
			myStates[USE_ALPHA_DELTA] = TRUE;
		}

		if (myParticleEmitterData->myIsAffectedByGravity == true)
		{
			myStates[AFFECTED_BY_GRAVITY] = TRUE;
		}

		if (myParticleEmitterData->myIsHat == true)
		{
			myStates[IS_HAT] = TRUE;
		}

		myTexture = myParticleEmitterData->myTexture;
		myEmissiveTexture = myParticleEmitterData->myEmissiveTexture;
	}

	void ParticleEmitterInstance::Reset()
	{
		mySceneTexture = myParticleEmitterData->myEffect->GetEffect()->GetVariableByName("SceneTexture")->AsShaderResource();
		DL_ASSERT_EXP(mySceneTexture->IsValid() == TRUE, "Failed to get the SceneTexture Variable in Particles!");

		myOffsetTexture = myParticleEmitterData->myEffect->GetEffect()->GetVariableByName("OffsetTexture")->AsShaderResource();
		DL_ASSERT_EXP(myOffsetTexture->IsValid() == TRUE, "Failed to get the OffsetTexture Variable in Particles!");




		for (int i = 0; i < myGraphicalParticles.GetCapacity(); ++i)
		{
			myGraphicalParticles[i] = GraphicalParticle();
			myLogicalParticles[i] = LogicalParticle();
		}
		myOffset = TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/Particle/T_Offset.dds");
		myLiveParticleCount = 0;

		myDiffColor = (myParticleEmitterData->myData.myEndColor - myParticleEmitterData->myData.myStartColor)
			/ myParticleEmitterData->myData.myParticleLifeTime;

		CheckFlags();

		myEmitterLife = myParticleEmitterData->myEmitterLifeTime;
		myEmissionTime = 0;
	}

	void ParticleEmitterInstance::CreateVertexBuffer()
	{

		myVertexWrapper = new VertexBufferWrapper();
		myVertexWrapper->myStride = sizeof(GraphicalParticle);
		myVertexWrapper->myByteOffset = 0;
		myVertexWrapper->myStartSlot = 0;
		myVertexWrapper->myNumberOfBuffers = 1;

		HRESULT hr;
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		if (myVertexWrapper->myVertexBuffer != nullptr)
		{
			myVertexWrapper->myVertexBuffer->Release();
		}

		vertexBufferDesc.ByteWidth = sizeof(GraphicalParticle) * myGraphicalParticles.Size();

		hr = Engine::GetInstance()->GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &myVertexWrapper->myVertexBuffer);
		DL_ASSERT_EXP(hr == S_OK, "[ParticleEmitterInstance](CreateVertexBuffer) : Failed to create VertexBuffer");

		Engine::GetInstance()->SetDebugName(myVertexWrapper->myVertexBuffer
			, "ParticleEmitterInstance::myVertexWrapper->myVertexBuffer");

	}

	int ParticleEmitterInstance::UpdateVertexBuffer()
	{
		myParticleToGraphicsCard.RemoveAll();
		for (int i = 0; i < myGraphicalParticles.Size(); ++i)
		{
			if (myGraphicalParticles[i].myAlpha > 0.0f)
			{
				myParticleToGraphicsCard.Add(myGraphicalParticles[i]);
			}
		}

		if (myParticleToGraphicsCard.Size() <= 0)
		{
			return 0;
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		Engine::GetInstance()->GetContex()->Map(myVertexWrapper->myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (mappedResource.pData != nullptr)
		{
			GraphicalParticle *data = static_cast<GraphicalParticle*>(mappedResource.pData);

			bool isSafe = sizeof(GraphicalParticle) == sizeof(myParticleToGraphicsCard[0]);
			DL_ASSERT_EXP(isSafe, "[ParticleEmitter](UpdateVertexBuffer) : Not safe to copy.");
			memcpy(data, &myParticleToGraphicsCard[0], sizeof(GraphicalParticle)* myParticleToGraphicsCard.Size());
		}

		Engine::GetInstance()->GetContex()->Unmap(myVertexWrapper->myVertexBuffer, 0);


		return myParticleToGraphicsCard.Size();
	}

	void ParticleEmitterInstance::UpdateEmitter(float aDeltaTime)
	{
		if (myStates[ACTIVE] == TRUE)
		{
			CheckFlags();
			myEmissionTime -= aDeltaTime;
			myEmitterLife -= aDeltaTime;

			if (myEntity != nullptr)
			{
				if (myEntity->GetComponent<InputComponent>() != nullptr && myStates[IS_HAT] == FALSE)
				{
					myOrientation.SetPos(myEntity->GetComponent<InputComponent>()->GetParticleOrientation().GetPos());
					myDirection = -myEntity->GetComponent<InputComponent>()->GetParticleOrientation().GetUp();
					myOverrideDirection = true;
				}
				if (myEntity->GetComponent<PlayerGraphicsComponent>() != nullptr && myStates[IS_HAT] == TRUE)
				{
					if (myEntity->GetComponent<PlayerGraphicsComponent>()->GetHead().myHat != nullptr 
						&& HatManager::GetInstance()->GetHatIDOnPlayer(myEntity->GetComponent<InputComponent>()->GetPlayerID()) == 12)
					{
						myOrientation = myEntity->GetComponent<PlayerGraphicsComponent>()->GetHead().myHat->myOrientation;
						myDirection = -myEntity->GetComponent<PlayerGraphicsComponent>()->GetHead().myHat->myOrientation.GetRight() +
							myEntity->GetComponent<PlayerGraphicsComponent>()->GetHead().myHat->myOrientation.GetUp();
						CU::Normalize(myDirection);
						myOverrideDirection = true;
					}
				}
			}

			if (myEmissionTime <= 0.f && (myEmitterLife > 0.f || myStates[EMITTERLIFE] == FALSE))
			{
				if (myStates[IS_HAT] == FALSE)
				{
					EmitParticle(myOrientation);
					myEmissionTime = myParticleEmitterData->myEmissionRate;
				}
			}
			
			if (myStates[IS_HAT] == true && myEmissionTime < 0.f)
			{
				if (myEntity->GetIsAlive() == true && myEntity->GetComponent<PlayerGraphicsComponent>()->GetHeadActive() == true)
				{
					EmitParticle(myOrientation);
					myEmissionTime = myParticleEmitterData->myEmissionRate;
				}
			}

			UpdateParticle(aDeltaTime);

			if (myStates[EMITTERLIFE] == TRUE)
			{
				if (myEmitterLife <= 0.f && myLiveParticleCount <= 0)
				{
					myStates[ACTIVE] = FALSE;
				}
			}
		}
	}

	void ParticleEmitterInstance::UpdateParticle(float aDeltaTime)
	{
		for (int i = 0; i < myLogicalParticles.Size(); ++i)
		{
			LogicalParticle& logicParticle = myLogicalParticles[i];

			if (logicParticle.myIsAlive == false)
			{
				continue;
			}

			GraphicalParticle& gfxParticle = myGraphicalParticles[i];
			ParticleData& particleData = myParticleEmitterData->myData;

			gfxParticle.myLifeTime -= aDeltaTime;

			if (particleData.mySpeedDelta > 0.f)
			{
				logicParticle.mySpeed += particleData.mySpeedDelta * aDeltaTime;
			}

			if (myStates[AFFECTED_BY_GRAVITY] == TRUE)
			{
				logicParticle.myDirection.y -= (9.82f * 0.1f) * aDeltaTime;
				CalcRotation(logicParticle.myDirection);
				gfxParticle.myRotation = myRotationToOverrideWith;
			}


			gfxParticle.myPosition.x += (logicParticle.myDirection.x * logicParticle.mySpeed) * aDeltaTime;
			gfxParticle.myPosition.z += (logicParticle.myDirection.z * logicParticle.mySpeed) * aDeltaTime;
			gfxParticle.myPosition.y += (logicParticle.myDirection.y * logicParticle.mySpeed) * aDeltaTime;




			gfxParticle.mySize += particleData.mySizeDelta * aDeltaTime;

			if (myStates[USE_ALPHA_DELTA] == TRUE)
			{
				gfxParticle.myAlpha += particleData.myAlphaDelta * aDeltaTime;
			}
			else if (myStates[USE_ALPHA_DELTA] == FALSE)
			{
				gfxParticle.myAlpha += particleData.myAlphaDelta * aDeltaTime;
				gfxParticle.myAlpha = gfxParticle.myLifeTime / particleData.myParticleLifeTime;
			}
			if (myStates[IS_HAT] == FALSE)
			{
				gfxParticle.myColor.x += myDiffColor.x  * aDeltaTime;
				gfxParticle.myColor.y += myDiffColor.y  * aDeltaTime;
				gfxParticle.myColor.z += myDiffColor.z  * aDeltaTime;
			}
			gfxParticle.myRotation += gfxParticle.myRotation * (logicParticle.myRotationDelta * aDeltaTime);


			if ((gfxParticle.myAlpha < 0.0f && gfxParticle.myLifeTime < 0.0f) && logicParticle.myIsAlive == true)
			{
				myLiveParticleCount--;
				logicParticle.myIsAlive = false;
				continue;
			}
		}
	}

	void ParticleEmitterInstance::EmitParticle(const CU::Matrix44f& aWorldMatrix)
	{
		for (int i = 0; i < myParticleEmitterData->myParticlesPerEmitt; ++i)
		{
			if (myParticleIndex == myLogicalParticles.Size() - 1)
			{
				myParticleIndex = 0;
			}
			myLiveParticleCount++;

			if (myParticleIndex >= myLogicalParticles.Size())
			{
				return;
			}

			GraphicalParticle &gfxParticle = myGraphicalParticles[myParticleIndex];
			LogicalParticle &logicParticle = myLogicalParticles[myParticleIndex];

			gfxParticle.myColor = myParticleEmitterData->myData.myStartColor;
			if (myStates[IS_HAT] == TRUE)
			{
				int r = CU::Math::RandomRange(1, 4);
				if (r == 1)
				{
					gfxParticle.myColor = CU::Vector3f(255.f, 0.f, 0.f);

				}
				else if (r == 2)
				{
					gfxParticle.myColor = CU::Vector3f(0, 255.f, 0.f);
				}
				if (r == 3)
				{
					gfxParticle.myColor = CU::Vector3f(0.f, 0.f, 255.f);
				}
			}

			if (myStates[AFFECTED_BY_GRAVITY] == true)
			{
				logicParticle.mySpeed = CU::Math::RandomRange(0.5f, myParticleEmitterData->myData.mySpeed);
			}
			else
			{
				logicParticle.mySpeed = myParticleEmitterData->myData.mySpeed;
			}
			logicParticle.myDirection = myDirection;

			if (myRandomizeDirection == true)
			{
				float randomAngle = CU::Math::RandomRange(-45.f, 45.f) * (3.1415926535f / 180.f);

				CU::Vector3f newDirection = { (myDirection.x * cos(-randomAngle)) - (myDirection.y * sin(-randomAngle))
					, (myDirection.y * cos(-randomAngle)) - (myDirection.x * sin(-randomAngle)), 0.f };
				CU::Normalize(newDirection);
				logicParticle.myDirection = newDirection;
			}

			if (myOverrideDirection == false)
			{

				logicParticle.myDirection = CU::Math::RandomVector(myParticleEmitterData->myMinDirection
					, myParticleEmitterData->myMaxDirection);
			}

#pragma	region		Shape
			if (myStates[CIRCLE] == TRUE && myStates[HOLLOW] == TRUE)
			{
				CU::Vector3<float> pos = CreateCirclePositions();
				gfxParticle.myPosition = aWorldMatrix.GetPos() + pos;
			}
			else if (myStates[CIRCLE] == TRUE)
			{
				CU::Vector3<float> pos = CreateCirclePositions();
				gfxParticle.myPosition = CU::Math::RandomVector(aWorldMatrix.GetPos() - pos
					, aWorldMatrix.GetPos() + pos);
			}
			else if (myStates[HOLLOW] == TRUE)
			{
				CU::Vector3<float> pos = CreateHollowSquare();
				gfxParticle.myPosition = aWorldMatrix.GetPos() + pos;
			}
			else if (myStates[SPHERE] == TRUE)
			{
				CU::Vector3<float> pos = CreateSpherePositions();
				gfxParticle.myPosition = aWorldMatrix.GetPos() + pos;
			}
			else
			{
				gfxParticle.myPosition =
					CU::Math::RandomVector(aWorldMatrix.GetPos() - myParticleEmitterData->myEmitterSize
					, aWorldMatrix.GetPos() + myParticleEmitterData->myEmitterSize);
			}
#pragma endregion

			gfxParticle.myLifeTime = myParticleEmitterData->myData.myParticleLifeTime;

			gfxParticle.myAlpha = myParticleEmitterData->myData.myStartAlpha;

			gfxParticle.mySize = CU::Math::RandomRange(myParticleEmitterData->myData.myMinStartSize
				, myParticleEmitterData->myData.myMaxStartSize);

			logicParticle.myIsAlive = true;

			gfxParticle.myRotation = myRotationToOverrideWith;
			if (myOverrideRotation == false)
			{
				logicParticle.myRotation = CU::Math::RandomRange(myParticleEmitterData->myParticleRotation.x, myParticleEmitterData->myParticleRotation.y);
				gfxParticle.myRotation = logicParticle.myRotation;
			}

			logicParticle.myRotationDelta = myParticleEmitterData->myRotationDelta;
			myParticleIndex += 1;
		}
	}

	CU::Vector3<float> ParticleEmitterInstance::CreateCirclePositions()
	{
		CU::Vector3f toReturn = CreateSpherePositions();
		toReturn.y = 0.f;
		CU::Vector3f toNormalize = toReturn - myOrientation.GetPos();
		CU::Normalize(toNormalize);
		myLogicalParticles[myParticleIndex].myDirection = toReturn;
		return toReturn;
	}

	CU::Vector3<float> ParticleEmitterInstance::CreateSpherePositions()
	{
		CU::Vector3<float> toReturn;

		toReturn = CU::Math::RandomVector(-myParticleEmitterData->myEmitterSize, myParticleEmitterData->myEmitterSize);
		CU::Normalize(toReturn);
		return toReturn;
	}

	CU::Vector3<float> ParticleEmitterInstance::CreateHollowSquare()
	{
		CU::Vector3<float> toReturn;
		CU::Vector3<float> size = myParticleEmitterData->myEmitterSize;
		int randomPoint = rand() % 4 + 1;

		switch (randomPoint)
		{
		case 1: //top
			toReturn.x = CU::Math::RandomRange(-size.x, size.x);
			toReturn.y = size.y;
			toReturn.z = 0.f;
			break;
		case 2: //bot
			toReturn.x = -size.x;
			toReturn.y = CU::Math::RandomRange(-size.y, size.y);
			toReturn.z = 0.f;
			break;
		case 3: //bot
			toReturn.x = CU::Math::RandomRange(-size.x, size.x);
			toReturn.y = -size.y;
			toReturn.z = 0.f;
			break;
		case 4: //top
			toReturn.x = size.x;
			toReturn.y = CU::Math::RandomRange(-size.z, size.z);
			toReturn.z = 0.f;
			break;
		default:
			break;
		}

		return toReturn;
	}

	void ParticleEmitterInstance::Activate(bool)
	{
		Reset();
		myStates[ACTIVE] = TRUE;
	}

	void ParticleEmitterInstance::KillEmitter(float aKillTime)
	{
		myEmitterLife = aKillTime;
		myStates[EMITTERLIFE] = TRUE;
		if (myEntity != nullptr)
		{
			myEntity = nullptr;
		}
	}

	void ParticleEmitterInstance::SetEntity(Entity* anEntity)
	{
		myEntity = anEntity;
	}

	CU::Vector3<float> ParticleEmitterInstance::GetPosition() const
	{
		return myOrientation.GetPos();
	}

	void ParticleEmitterInstance::SetRandomizeDirection(bool aShouldBeSet)
	{
		myRandomizeDirection = aShouldBeSet;
	}

	void ParticleEmitterInstance::SetOtherOrientation(const CU::Matrix44f& aMatrix)
	{
		myOtherOrientation = aMatrix;
	}

	void ParticleEmitterInstance::CalcRotation(const CU::Vector3f& aDirectionToCalcFrom)
	{
		float dot = CU::Dot(aDirectionToCalcFrom, CU::Vector3f(0.f, 1.f, 0.f));
		float det = CU::Dot(CU::Vector3f(0.f, 0.f, 1.f), CU::Cross(aDirectionToCalcFrom, CU::Vector3f(0.f, 1.f, 0.f)));

		float angle = -atan2(det, dot);

		myRotationToOverrideWith = angle;
		myOverrideRotation = true;
	}
}