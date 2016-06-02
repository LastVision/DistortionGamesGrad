#pragma once

#include "ParticleData.h"

struct ID3D11InputLayout;
struct _D3DX11_TECHNIQUE_DESC;

class XMLReader;
namespace tinyxml2
{
	class XMLElement;
}

namespace Prism
{
	class ParticleEmitterInstance;
	class Texture;
	class Effect;


	class ParticleEmitterData
	{
	
		friend class ParticleEmitterInstance;
		friend class ParticleDataContainer;

	public:
		ParticleEmitterData();
		~ParticleEmitterData();

		void LoadDataFile(const std::string& aFilePath);
		void ReloadDataFile();

	private:

		void CreateInputLayout();
		

		void ReadParticleData(XMLReader* aReader, tinyxml2::XMLElement* anElement);
		void ReadEmitterData(XMLReader* aReader, tinyxml2::XMLElement* anElement);



		Texture*				myTexture = nullptr;
		Texture*				myEmissiveTexture = nullptr;
		Effect*					myEffect = nullptr; 
		ID3D11InputLayout*		myInputLayout = nullptr;
		_D3DX11_TECHNIQUE_DESC*	myTechniqueDesc = nullptr;


		ParticleData			myData;
		std::string				myFilePath ="not initialized";
		int						myMaxParticleAmount = 0;

		std::string	myEffectName = "not initialized";
		std::string	myTextureName = "not initialized";
		std::string	myEmissiveName = "not initialized";

		CU::Vector3f myEmitterSize;
		CU::Vector3f myMinDirection;
		CU::Vector3f myMaxDirection;

		CU::Vector2<float> myParticleRotation;
		CU::Vector2<float> myMinMaxSpeed;

		float myRotationDelta = 0.f;
		float myEmitterLifeTime = 0.f;
		float myEmissionRate = 0.f;

		int	myParticlesPerEmitt = 0;

		bool myUseEmitterLifeTime = false;
		bool myUseAlphaDelta = false;
		bool myIsActiveAtStart = false;
		bool myIsHollow = false;
		bool myIsCircle = false;
		bool myIsAnimated = false;
		bool myIsSphere = false;
		bool myIsAffectedByGravity = false;
	};
}