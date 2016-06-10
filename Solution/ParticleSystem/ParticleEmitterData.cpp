#include "stdafx.h"

#include <d3dx11effect.h>
#include <Effect.h>
#include <EffectContainer.h>
#include <Engine.h>
#include "ParticleEmitterData.h"
#include <TextureContainer.h>
#include <XMLReader.h>

namespace Prism
{
	ParticleEmitterData::ParticleEmitterData()
	{
	}

	ParticleEmitterData::~ParticleEmitterData()
	{
		if (myInputLayout != nullptr)
		{
			SAFE_RELEASE(myInputLayout);
		}
		SAFE_DELETE(myTechniqueDesc);
	}

	void ParticleEmitterData::LoadDataFile(const std::string& aFilePath)
	{
		myFilePath = aFilePath;
		ReloadDataFile();
		myEffect = EffectContainer::GetInstance()->GetEffect(myEffectName);

		CreateInputLayout();
		myTechniqueDesc = new _D3DX11_TECHNIQUE_DESC();
	}

	void ParticleEmitterData::ReloadDataFile()
	{
		SET_RUNTIME(false);
		XMLReader read;

		read.OpenDocument(myFilePath);
		tinyxml2::XMLElement* emitter = read.ForceFindFirstChild("Emitter");
		tinyxml2::XMLElement* element;
		element = read.ForceFindFirstChild(emitter, "Shader");
		read.ReadAttribute(element, "filepath", myEffectName);

		element = read.ForceFindFirstChild(emitter, "Texture");
		read.ReadAttribute(element, "filepath", myTextureName);


		element = read.ForceFindFirstChild(emitter, "EmissiveTexture");
		read.ReadAttribute(element, "filepath", myEmissiveName);

		ReadEmitterData(&read, emitter);
		ReadParticleData(&read, emitter);

		read.CloseDocument();

		myData.myStartColor /= 255.f;
		myData.myEndColor /= 255.f;

		myTexture = TextureContainer::GetInstance()->GetTexture(myTextureName);
		myEmissiveTexture = TextureContainer::GetInstance()->GetTexture(myEmissiveName);
		RESET_RUNTIME;
	}

	void ParticleEmitterData::ReadParticleData(XMLReader* aReader, tinyxml2::XMLElement* anElement)
	{
		tinyxml2::XMLElement* element = nullptr;
	
		element = aReader->ForceFindFirstChild(anElement, "ParticleScale");
		aReader->ForceReadAttribute(element, "min", myData.myMinStartSize);
		aReader->ForceReadAttribute(element, "max", myData.myMaxStartSize);
		myData.myMinStartSize /= 100.f;
		myData.myMaxStartSize /= 100.f;

		element = aReader->ForceFindFirstChild(anElement, "ParticleSizeDelta");
		aReader->ForceReadAttribute(element, "value", myData.mySizeDelta);
		myData.mySizeDelta /= 100.f;

		element = aReader->ForceFindFirstChild(anElement, "ParticleAlphaStart");
		aReader->ForceReadAttribute(element, "value", myData.myStartAlpha);

		element = aReader->ForceFindFirstChild(anElement, "ParticleAlphaDelta");
		aReader->ForceReadAttribute(element, "value", myData.myAlphaDelta);

		element = aReader->ForceFindFirstChild(anElement, "ParticleSpeed");
		aReader->ForceReadAttribute(element, "value", myData.mySpeed);
		myData.mySpeed /= 100.f;

		element = aReader->ForceFindFirstChild(anElement, "ParticleSpeedDelta");
		aReader->ForceReadAttribute(element, "value", myData.mySpeedDelta);
		myData.mySpeedDelta /= 100.f;

		element = aReader->ForceFindFirstChild(anElement, "ParticleStartColor");
		aReader->ForceReadAttribute(element, "r", "g", "b", myData.myStartColor);

		element = aReader->ForceFindFirstChild(anElement, "ParticleEndColor");
		aReader->ForceReadAttribute(element, "r", "g", "b", myData.myEndColor);

		element = aReader->ForceFindFirstChild(anElement, "ParticleLifeTime");
		aReader->ForceReadAttribute(element, "value", myData.myParticleLifeTime);

		element = aReader->ForceFindFirstChild(anElement, "ParticleRotation");
		aReader->ForceReadAttribute(element, "min", myParticleRotation.x);
		aReader->ForceReadAttribute(element, "max", myParticleRotation.y);

		element = aReader->ForceFindFirstChild(anElement, "ParticleRotationDelta");
		aReader->ForceReadAttribute(element, "value", myRotationDelta);

		element = aReader->ForceFindFirstChild(anElement, "ParticlesPerEmitt");
		aReader->ForceReadAttribute(element, "value", myParticlesPerEmitt);

	}

	void ParticleEmitterData::ReadEmitterData(XMLReader* aReader, tinyxml2::XMLElement* anElement)
	{
		tinyxml2::XMLElement* element = nullptr;
		element = aReader->ForceFindFirstChild(anElement, "EmitterFlags");

		aReader->ForceReadAttribute(element, "activeAtStart", myIsActiveAtStart);
		aReader->ForceReadAttribute(element, "circleEmitter", myIsCircle);
		aReader->ForceReadAttribute(element, "hollowEmitter", myIsHollow);
		aReader->ForceReadAttribute(element, "affectedByGravity", myIsAffectedByGravity);
		aReader->ReadAttribute(element, "heatHaze", myHasHeatHaze);
		aReader->ReadAttribute(element, "isHat", myIsHat);

		element = aReader->ForceFindFirstChild(anElement, "EmitterSize");
		aReader->ForceReadAttribute(element, "x", "y", "z", myEmitterSize);
		myEmitterSize *= 0.5f;

		element = aReader->ForceFindFirstChild(anElement, "EmitterLifeTime");
		aReader->ForceReadAttribute(element, "value", myEmitterLifeTime);
		if (myEmitterLifeTime < 0)
		{
			myUseEmitterLifeTime = false;
		}
		else
		{
			myUseEmitterLifeTime = true;
		}

		element = aReader->ForceFindFirstChild(anElement, "MinDirection");
		aReader->ForceReadAttribute(element, "x", "y", "z", myMinDirection);

		element = aReader->ForceFindFirstChild(anElement, "MaxDirection");
		aReader->ForceReadAttribute(element, "x", "y", "z", myMaxDirection);

		element = aReader->ForceFindFirstChild(anElement, "EmittsPerSecond");
		aReader->ForceReadAttribute(element, "value", myEmissionRate);
		myEmissionRate = 1 / myEmissionRate;

		element = aReader->ForceFindFirstChild(anElement, "UseAlphaDelta");
		aReader->ForceReadAttribute(element, "value", myUseAlphaDelta);
	}

	void ParticleEmitterData::CreateInputLayout()
	{
		HRESULT hr;
		D3DX11_PASS_DESC passDesc;
		if (myHasHeatHaze == false)
		{
			hr = myEffect->GetTechnique("Render")->GetPassByIndex(0)->GetDesc(&passDesc);
		}
		else
		{
			hr = myEffect->GetTechnique("Render_HeatHaze")->GetPassByIndex(0)->GetDesc(&passDesc);
		}
		DL_ASSERT_EXP(!FAILED(hr), "[ParticleEmitterData](CreateInputLayout) : Failed to get Pass Description!");

		const D3D11_INPUT_ELEMENT_DESC VertexParticleLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "ALPHA", 0, DXGI_FORMAT_R32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TIME", 0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "ROTATION", 0, DXGI_FORMAT_R32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		UINT size = ARRAYSIZE(VertexParticleLayout);

		hr = Engine::GetInstance()->GetDevice()->CreateInputLayout(VertexParticleLayout
			, size
			, passDesc.pIAInputSignature
			, passDesc.IAInputSignatureSize
			, &myInputLayout);
		DL_ASSERT_EXP(!FAILED(hr), "[ParticleEmitterData](CreateInputLayout) : Failed to Create InputLayout!");
		Engine::GetInstance()->SetDebugName(myInputLayout, "ParticleEmitterData::myInputLayout");
	}

}