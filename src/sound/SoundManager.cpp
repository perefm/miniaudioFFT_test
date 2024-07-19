// SoundManager.cpp
// Spontz Demogroup

#include "main.h"
#include "sound/SoundManager.h"

namespace Phoenix {

	SoundManager::SoundManager()
		:
		m_channels(CHANNEL_COUNT),
		m_sampleRate(SAMPLE_RATE),
		m_pDevice(nullptr),
		m_pOutputFFTF32(nullptr),
		m_pSampleBuf(nullptr),
		m_pFFTBuffer(nullptr),
		m_pFFTFrequencies(nullptr)
	{
		ma_result result;

		sound.clear();
		m_LoadedSounds = 0;
		m_inited = false;

		// Setup FFT variables
		// Sample buffer
		m_pSampleBuf = (float*)malloc(sizeof(float) * FFT_SIZE * 2);
		if (m_pSampleBuf != nullptr)
			memset(m_pSampleBuf, 0, sizeof(float) * FFT_SIZE * 2);
		
		// FFT config
		m_fftcfg = kiss_fftr_alloc(FFT_SIZE * 2, false, NULL, NULL);
				
		// FFT values buffer
		m_pFFTBuffer = (float*)malloc(sizeof(float) * FFT_SIZE);
		if (m_pFFTBuffer)
			memset(m_pFFTBuffer, 0, sizeof(float) * FFT_SIZE);
		
		// FFT frequencies buffer
		m_pFFTFrequencies = (float*)malloc(sizeof(float) * FFT_SIZE);
		if (m_pFFTFrequencies)
		for (int32_t i = 0; i < FFT_SIZE; i++) {
			m_pFFTFrequencies[i] = static_cast<float>(i) * (SAMPLE_RATE / FFT_SIZE);
		}
		
		// FFT output buffer
		m_pOutputFFTF32 = (float*)malloc(sizeof(float) * SAMPLE_STORAGE);
		if (m_pOutputFFTF32)
			memset(m_pOutputFFTF32, 0, sizeof(float) * SAMPLE_STORAGE);

		// Allocate space for structure
		m_pDevice = (ma_device*)malloc(sizeof(ma_device));

		// Init the device
		ma_device_config deviceConfig;
		deviceConfig = ma_device_config_init(ma_device_type_playback);
		deviceConfig.playback.format = SAMPLE_FORMAT;
		deviceConfig.playback.channels = m_channels;
		deviceConfig.sampleRate = m_sampleRate;
		deviceConfig.dataCallback = dataCallback;
		deviceConfig.pUserData = this;

		result = ma_device_init(NULL, &deviceConfig, m_pDevice);
		if (result != MA_SUCCESS) {
			// Failed to open playback device
			destroyDevice();
			m_inited = false;
			return;
		}

		// We can't stop in the audio thread so we instead need to use an event. We wait on this thread in the main thread, and signal it in the audio thread. This
		// needs to be done before starting the device. We need a context to initialize the event, which we can get from the device. Alternatively you can initialize
		// a context separately, but we don't need to do that for this example.
		ma_event_init(&m_stopEvent);

		m_inited = true;
	}

	SoundManager::~SoundManager()
	{
		ma_event_signal(&m_stopEvent);	// Send the signal to stop
		ma_event_wait(&m_stopEvent);	// Wait the stop
		destroyDevice();
		clearSounds();
		kiss_fft_free(m_fftcfg);		// Free fft

		// Delete internal buffers
		if (m_pSampleBuf)
			free(m_pSampleBuf);
		if (m_pOutputFFTF32)
			free(m_pOutputFFTF32);
		if (m_pFFTBuffer)
			free(m_pFFTBuffer);
		if (m_pFFTFrequencies)
			free(m_pFFTFrequencies);
	}

	void SoundManager::destroyDevice()
	{
		if (m_pDevice) {
			ma_device_stop(m_pDevice);
			ma_device_uninit(m_pDevice);
			free(m_pDevice);
			m_pDevice = nullptr;
		}
	}

	bool SoundManager::setMasterVolume(float volume)
	{
		if (m_inited && m_pDevice) {
			ma_device_set_master_volume(m_pDevice, volume);
			return true;
		}
		return false;
	}

	SP_Sound SoundManager::addSound(const std::string_view filePath)
	{
		SP_Sound p_sound;

		// check if sound is already loaded, then we just retrieve the ID of our sound
		for (auto const& m_sound : sound) {
			if (m_sound->filePath.compare(filePath) == 0) {
				p_sound = m_sound;
			}
		}

		if (p_sound == nullptr) {
			SP_Sound new_sound = std::make_shared<Sound>();
			if (new_sound->loadSoundFile(filePath, m_channels, m_sampleRate)) {
				sound.push_back(new_sound);
				printf("\nSound %s loaded OK", filePath.data());
				m_LoadedSounds++;
				p_sound = new_sound;
			}
			else {
				printf("\nCould not load song: %s", filePath.data());
			}
		}
		return p_sound;

	}

	SP_Sound SoundManager::getSoundbyID(uint32_t id)
	{
		if (id >= sound.size())
			return nullptr;
		else
			return sound[id];
	}

	void SoundManager::clearSounds()
	{
		sound.clear();
		m_LoadedSounds = 0;
	}

	std::string SoundManager::getVersion()
	{
		std::string ma_version;
		ma_version = MA_VERSION_STRING;
		return ma_version;
	}

	void SoundManager::playDevice()
	{
		if (m_pDevice)
			ma_device_start(m_pDevice);
	}

	void SoundManager::stopDevice()
	{
		if (m_pDevice)
			ma_device_stop(m_pDevice);
	}

	void SoundManager::enumerateDevices()
	{
		ma_result result;
		ma_context context;
		ma_device_info* pPlaybackDeviceInfos;
		ma_uint32 playbackDeviceCount;
		ma_device_info* pCaptureDeviceInfos;
		ma_uint32 captureDeviceCount;
		ma_uint32 iDevice;

		if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
			printf("Failed to initialize context.\n");
			return;
		}

		result = ma_context_get_devices(&context, &pPlaybackDeviceInfos, &playbackDeviceCount, &pCaptureDeviceInfos, &captureDeviceCount);
		if (result != MA_SUCCESS) {
			printf("Failed to retrieve device information.\n");
			return;
		}

		printf("Playback Devices\n");
		for (iDevice = 0; iDevice < playbackDeviceCount; ++iDevice) {
			printf("    %u: %s\n", iDevice, pPlaybackDeviceInfos[iDevice].name);
		}

		printf("\n");

		printf("Capture Devices\n");
		for (iDevice = 0; iDevice < captureDeviceCount; ++iDevice) {
			printf("    %u: %s\n", iDevice, pCaptureDeviceInfos[iDevice].name);
		}

		ma_context_uninit(&context);

	}

	ma_uint32 SoundManager::read_and_mix_pcm_frames_f32(ma_decoder* pDecoder, float volume, float* pOutputF32, float* pOutputFFTF32, ma_uint32 frameCount)
	{
		// The way mixing works is that we just read into a temporary buffer, then take the contents of that buffer and mix it with the
		// contents of the output buffer by simply adding the samples together. You could also clip the samples to -1..+1, but I'm not
		//doing that in this example.
		ma_result result;
		std::vector<float> temp(SAMPLE_STORAGE);
		ma_uint32 tempCapInFrames = SAMPLE_STORAGE / CHANNEL_COUNT;
		ma_uint32 totalFramesRead = 0;

		while (totalFramesRead < frameCount) {
			ma_uint64 iSample;
			ma_uint64 iOutputSample;
			ma_uint64 framesReadThisIteration;
			ma_uint32 totalFramesRemaining = frameCount - totalFramesRead;
			ma_uint32 framesToReadThisIteration = tempCapInFrames;
			if (framesToReadThisIteration > totalFramesRemaining) {
				framesToReadThisIteration = totalFramesRemaining;
			}

			result = ma_decoder_read_pcm_frames(pDecoder, temp.data(), framesToReadThisIteration, &framesReadThisIteration);
			if (result != MA_SUCCESS || framesReadThisIteration == 0) {
				break;
			}


			/* Mix the frames together. */
			for (iSample = 0; iSample < framesReadThisIteration * CHANNEL_COUNT; ++iSample) {
				iOutputSample = totalFramesRead * CHANNEL_COUNT + iSample;
				pOutputF32[iOutputSample] += temp[iSample] * volume;
				pOutputFFTF32[iOutputSample] += temp[iSample];
			}

			totalFramesRead += (ma_uint32)framesReadThisIteration;

			if (framesReadThisIteration < (ma_uint32)framesToReadThisIteration) {
				break;  /* Reached EOF. */
			}
		}

		return totalFramesRead;
	}


	void SoundManager::dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
	{
		(void)pInput; // added to avoid compiler warnings. It does nothing.
		float* pOutputF32 = (float*)pOutput;
		SoundManager* p_sm = (SoundManager*)pDevice->pUserData;
		memset(p_sm->m_pOutputFFTF32, 0, sizeof(float) * SAMPLE_STORAGE);

		for (auto const& mySound : (p_sm->sound)) {
			if (mySound->status == Sound::State::Playing) {
				ma_uint32 framesRead = read_and_mix_pcm_frames_f32(mySound->getDecoder(), mySound->volume, pOutputF32, p_sm->m_pOutputFFTF32, frameCount);
				if (framesRead < frameCount) {
					mySound->stopSound();
				}
			}
		}

		// Fill the sampleBuffer for the FFT analysis
		frameCount = frameCount < FFT_SIZE * 2 ? frameCount : FFT_SIZE * 2;

		// Just rotate the buffer; copy existing, append new - https://github.com/Gargaj/Bonzomatic/blob/master/src/platform_common/FFT.cpp
		const float* samples = (const float*)p_sm->m_pOutputFFTF32;
		if (samples) {
			float* p_sample = p_sm->m_pSampleBuf;
			for (uint32_t i = frameCount; i < (FFT_SIZE * 2); i++) {
				*(p_sample++) = p_sm->m_pSampleBuf[i];
			}
			for (uint32_t i = 0; i < frameCount; i++) {
				*(p_sample++) = (samples[i * 2] + samples[i * 2 + 1]) / 2.0f * p_sm->m_fAmplification;
			}
		}
	}

	bool SoundManager::performFFT()
	{
		if (!m_inited) {
			return false;
		}

		kiss_fft_cpx out[FFT_SIZE + 1];			// FFT complex output
		kiss_fftr(m_fftcfg, m_pSampleBuf, out);


		m_lowFreqSum = 0.0f;
		m_midFreqSum = 0.0f;
		m_highFreqSum = 0.0f;

		for (uint32_t i = 0; i < FFT_SIZE; i++)
		{
			// Calculate the FFT buffer
			static const float scaling = 1.0f / (float)FFT_SIZE;
			m_pFFTBuffer[i] = 2.0f * sqrtf(out[i].r * out[i].r + out[i].i * out[i].i) * scaling;

			// Calculate the maximum value of the Low, Medium and High frequencies
			if (m_pFFTFrequencies[i] <= m_lowFreqMax) {
				m_lowFreqSum += m_pFFTBuffer[i];
			}
			else if (m_pFFTFrequencies[i] <= m_midFreqMax) {
				m_midFreqSum += m_pFFTBuffer[i];
			}
			else {
				m_highFreqSum += m_pFFTBuffer[i];
			}
		}

		return true;
	}

}
