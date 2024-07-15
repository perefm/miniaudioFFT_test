// SoundManager.h
// Spontz Demogroup

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <memory>

#include <kiss_fft.h>
#include <kiss_fftr.h>

#include "sound/Sound.h"

namespace Phoenix {

	#define FFT_SIZE 1024

	class SoundManager final {

	public:
		SoundManager();
		~SoundManager();

	public:
		bool initEngine();
		void uninitEngine();
		bool setVolume(float volume);
		SP_Sound addSound(const std::string_view filePath);
		SP_Sound getSoundbyID(uint32_t id);
		void clearSounds(); // Clear all sounds
		std::string getVersion();

		void enumerateDevices();

	private:
		//static void dataCallback (ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

	private:
		ma_engine				m_engine;
		int32_t					m_LoadedSounds; // Loaded sounds
		bool					m_inited;

		// Resource manager
		ma_resource_manager_config	m_resourceManagerConfig;
		ma_resource_manager			m_resourceManager;

		// FFT capture and analysis
		kiss_fftr_cfg				m_fftcfg;
		float						m_sampleBuf[FFT_SIZE * 2];
		float						m_fAmplification = 1.0f;

	public:
		std::vector<SP_Sound>	sound; // Sound list

	};
}
