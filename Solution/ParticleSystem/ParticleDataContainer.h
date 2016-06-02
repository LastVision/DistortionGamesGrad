#pragma once
#include <string>
#include <unordered_map>

#ifndef RELEASE_BUILD
#include <FileWatcher.h>
#endif

namespace Prism
{
	class Camera;
	class ParticleEmitterData;
	class Texture;
	class ParticleDataContainer
	{
	public:
		static ParticleDataContainer* GetInstance();
		static void Destroy();

		ParticleEmitterData*	GetParticleData(const std::string& aFilePath);
		void SetGPUData(const Camera& aCamera, Texture* aDepthTexture);
		void Update();
	private:
		ParticleDataContainer(){};
		~ParticleDataContainer();
		void LoadParticleData(const std::string& aFilePath);
		void ReloadParticleData(const std::string& aFilePath);


		std::unordered_map<std::string, ParticleEmitterData*> myParticleData;

		static ParticleDataContainer* myInstance;

#ifndef RELEASE_BUILD
		CU::FileWatcher myFileWatcher;
#endif
	};
};