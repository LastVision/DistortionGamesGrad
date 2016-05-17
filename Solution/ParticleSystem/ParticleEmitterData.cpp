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
		: myTexture(nullptr)
		, myEffect(nullptr)
		, myTechniqueDesc(nullptr)
		, myInputLayout(nullptr)
		, myFileName("not initialized")
		, myEffectName("not initialized")
		, myTextureName("not initialized")
		, myRotationDelta(0.f)
		, myEmitterLifeTime(0.f)
		, myEmissionRate(0.f)
		, myParticlesPerEmitt(0)
		, myMaxParticleAmount(0)
		, myIsActiveAtStart(false)
		, myUseEmitterLifeTime(false)
		, myIsHollow(false)
		, myIsCircle(false)
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
		XMLReader read;
		myFileName = aFilePath;

		read.OpenDocument(aFilePath);
		tinyxml2::XMLElement* emitter = read.ForceFindFirstChild("Emitter");
		tinyxml2::XMLElement* element;
		element = read.ForceFindFirstChild(emitter, "Shader");
		read.ReadAttribute(element, "filepath", myEffectName);

		element = read.ForceFindFirstChild(emitter, "Texture");
		read.ReadAttribute(element, "filepath", myTextureName);

		element = read.ForceFindFirstChild(emitter, "EmitterFlags");
		read.ReadAttribute(element, "activeAtStart", myIsActiveAtStart);
		read.ReadAttribute(element, "circleEmitter", myIsCircle);
		read.ReadAttribute(element, "hollowEmitter", myIsHollow);

		element = read.ForceFindFirstChild(emitter, "EmitterSize");
		read.ReadAttribute(element, "x", "y", "z", myEmitterSize);
		myEmitterSize *= 0.5f;

		element = read.ForceFindFirstChild(emitter, "EmitterLifeTime");
		read.ForceReadAttribute(element, "value", myEmitterLifeTime);
		if (myEmitterLifeTime < 0)
		{
			myUseEmitterLifeTime = false;
		}
		else
		{
			myUseEmitterLifeTime = true;
		}

		element = read.ForceFindFirstChild(emitter, "ParticleRotationDelta");
		read.ForceReadAttribute(element, "value", myRotationDelta);

		element = read.ForceFindFirstChild(emitter, "ParticleRotation");
		read.ForceReadAttribute(element, "min", myParticleRotation.x);
		read.ForceReadAttribute(element, "max", myParticleRotation.y);

		element = read.ForceFindFirstChild(emitter, "MinDirection");
		read.ForceReadAttribute(element, "x", "y", "z", myMinDirection);

		element = read.ForceFindFirstChild(emitter, "MaxDirection");
		read.ForceReadAttribute(element, "x", "y", "z", myMinDirection);

		element = read.ForceFindFirstChild(emitter, "ParticleStartColor");
		read.ForceReadAttribute(element, "r", "g", "b", myData.myStartColor);

		element = read.ForceFindFirstChild(emitter, "ParticleEndColor");
		read.ForceReadAttribute(element, "r", "g", "b", myData.myEndColor);

		element = read.ForceFindFirstChild(emitter, "ParticleLifeTime");
		read.ForceReadAttribute(element, "value", myData.myParticleLifeTime);

		element = read.ForceFindFirstChild(emitter, "EmittsPerSecond");
		read.ForceReadAttribute(element, "value", myEmissionRate);
		myEmissionRate = 1 / myEmissionRate;

		element = read.ForceFindFirstChild(emitter, "ParticlesPerEmitt");
		read.ForceReadAttribute(element, "value", myParticlesPerEmitt);

		element = read.ForceFindFirstChild(emitter, "ParticleScale");
		read.ForceReadAttribute(element, "min", myData.myMinStartSize);
		read.ForceReadAttribute(element, "max", myData.myMaxStartSize);

		element = read.ForceFindFirstChild(emitter, "ParticleSizeDelta");
		read.ForceReadAttribute(element, "value", myData.mySizeDelta);

		element = read.ForceFindFirstChild(emitter, "ParticleAlphaStart");
		read.ForceReadAttribute(element, "value", myData.myStartAlpha);

		element = read.ForceFindFirstChild(emitter, "ParticleAlphaDelta");
		read.ForceReadAttribute(element, "value", myData.myAlphaDelta);

		element = read.ForceFindFirstChild(emitter, "ParticleSpeed");
		read.ForceReadAttribute(element, "value", myData.mySpeed);

		element = read.ForceFindFirstChild(emitter, "ParticleSpeedDelta");
		read.ForceReadAttribute(element, "value", myData.mySpeedDelta);

		read.CloseDocument();

		myData.myStartColor /= 255.f;
		myData.myEndColor /= 255.f;

		myTexture = TextureContainer::GetInstance()->GetTexture(myTextureName.c_str());
		myEffect = EffectContainer::GetInstance()->GetEffect(myEffectName.c_str());

		CreateInputLayout();
		myTechniqueDesc = new _D3DX11_TECHNIQUE_DESC();
	}

	void ParticleEmitterData::CreateInputLayout()
	{
		HRESULT hr;
		D3DX11_PASS_DESC passDesc;
		hr = myEffect->GetTechnique("Render")->GetPassByIndex(0)->GetDesc(&passDesc);
		DL_ASSERT_EXP(!FAILED(hr), "[ParticleEmitterData](CreateInputLayout) : Failed to get Pass Description!");

		const D3D11_INPUT_ELEMENT_DESC VertexParticleLayout[] =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "ALPHA",		0, DXGI_FORMAT_R32_FLOAT,		0, 24,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SIZE",		0, DXGI_FORMAT_R32_FLOAT,		0, 28,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TIME",		0, DXGI_FORMAT_R32_FLOAT,		0, 32,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "ROTATION",	0, DXGI_FORMAT_R32_FLOAT,		0, 36,	D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

	void ParticleEmitterData::ReadParticleData(tinyxml2::XMLElement* anElement)
	{

	}

	void ParticleEmitterData::ReadEmitterData(tinyxml2::XMLElement* anElement)
	{

	}

}