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
	#define CHANNEL_COUNT 2
	#define SAMPLE_RATE 44100
	#define SAMPLE_FORMAT ma_format_f32
	#define SAMPLE_STORAGE	4096 // Sample storage size (4096 float samples)

	class SoundManager final {

	public:
		SoundManager();
		~SoundManager();

	public:
		bool setMasterVolume(float volume);
		SP_Sound addSound(const std::string_view filePath);
		SP_Sound getSoundbyID(uint32_t id);
		void clearSounds();
		std::string getVersion();

		void playDevice();
		void stopDevice();

		void enumerateDevices();

	private:
		static ma_uint32 read_and_mix_pcm_frames_f32(ma_decoder* pDecoder, float volume, float* pOutputF32, float* pOutputFFTF32, ma_uint32 frameCount);
		static void dataCallback (ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
		void destroyDevice();
	
	public:
		bool performFFT();

	private:
		ma_device*		m_pDevice;		// Internal miniaudio device for playback
		ma_event		m_stopEvent;	// Signaled by the audio thread, waited on by the main thread.

		int32_t			m_LoadedSounds; // Loaded sounds
		bool			m_inited;

		uint32_t		m_channels;
		uint32_t		m_sampleRate;
	
		// FFT capture and analysis
		kiss_fftr_cfg	m_fftcfg;
		float*			m_pSampleBuf;				// Sample buffer used to capture Buffer values, to be sent to the FFT analyzer, Size is: (FFT_SIZE * 2)
		float			m_fAmplification = 1.0f;
		float*			m_pOutputFFTF32;			// Buffer for storing the output samples, removing the impacts of the volume control, size is: SAMPLE_STORAGE
		
		// Group magnitudes into low, mid, and high frequency bands
		float			m_lowFreqMax = 400.0f;	// Low frequency max value
		float			m_midFreqMax = 2000.0f;	// Mid frequency max value

	public:
		float*			m_pFFTBuffer;		// FFT magnitues, size is: FFT_SIZE
		float*			m_pFFTFrequencies;	// FFT frequencies analyzed, size is: FFT_SIZE
		float			m_lowFreqSum = 0.0f;
		float			m_midFreqSum = 0.0f;
		float			m_highFreqSum = 0.0f;

	public:
		std::vector<SP_Sound>	sound; // Sound list

	};
}
