// Sound.cpp
// Spontz Demogroup

#include "main.h"
#include "sound/Sound.h"

namespace Phoenix {

	Sound::Sound()
		:
		m_pDecoder(nullptr),
		m_pDevice(nullptr),
		filePath(""),
		loaded(false)
	{
	}

	Sound::~Sound()
	{
		unLoadSong();
	}

	void Sound::unLoadSong()
	{
		if (m_pDevice) {
			ma_device_stop(m_pDevice);
			ma_device_uninit(m_pDevice);
			free(m_pDevice);
			m_pDevice = nullptr;
		}
		if (m_pDecoder) {
			ma_decoder_uninit(m_pDecoder);
			free(m_pDecoder);
			m_pDecoder = nullptr;
		}
		loaded = false;
	}

	void Sound::dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
	{
		Sound* p = (Sound*)pDevice->pUserData;

		if (!p || p->m_pDecoder == nullptr) {
			printf("\nwarning: sound object %p : pDevice=%p, pDecoder=%p\n", p, pDevice, p->m_pDecoder);
			return;
		}

		
		ma_decoder_read_pcm_frames(p->m_pDecoder, pOutput, frameCount, NULL);
	}

	bool Sound::loadSoundFile(const std::string_view soundFile, ma_engine& engine)
	{
		ma_result result;

		// If song is already loaded, we unload it first
		if (loaded) {
			unLoadSong();
		}
		filePath = soundFile;
		
		// Allocate space for structure
		m_pDevice = (ma_device*)malloc(sizeof(ma_device));
		m_pDecoder = (ma_decoder*)malloc(sizeof(ma_decoder));

		// Init de Decoder and load song
		ma_decoder_config decoderConfig;
		decoderConfig = ma_decoder_config_init(ma_format_f32, 2, 44100); // output format f32, 2 channels, 44100Hz (TODO: Parametrize this)
		result = ma_decoder_init_file(soundFile.data(), &decoderConfig, m_pDecoder);
		if (result != MA_SUCCESS) {
			unLoadSong();
			loaded = false;
			return loaded;
		}
		else {
			loaded = true;
		}
		
		// Init the device
		ma_device_config deviceConfig;

		deviceConfig = ma_device_config_init(ma_device_type_playback);
		deviceConfig.playback.format = m_pDecoder->outputFormat;
		deviceConfig.playback.channels = m_pDecoder->outputChannels;
		deviceConfig.sampleRate = m_pDecoder->outputSampleRate;
		deviceConfig.dataCallback = dataCallback;
		deviceConfig.pUserData = this;

		if (ma_device_init(NULL, &deviceConfig, m_pDevice) != MA_SUCCESS) {
			// Failed to open playback device
			unLoadSong();
			loaded = false;
			return loaded;
		}

		loaded = true;
		return loaded;
	}

	bool Sound::playSound()
	{
		ma_result result;
		if (loaded) {
			result = ma_device_start(m_pDevice);
			if (result != MA_SUCCESS) {
				return false;
			}
		}
		else
			return false;
		
		return true;
	}

	bool Sound::stopSound()
	{
		ma_result result;
		if (loaded) {
			result = ma_device_stop(m_pDevice);
			if (result != MA_SUCCESS) {
				return false;
			}
		}
		else
			return false;

		return true;
	}

	bool Sound::restartSound()
	{
		ma_result result;
		if (loaded) {
			result = ma_decoder_seek_to_pcm_frame(m_pDecoder, 0);
			if (result != MA_SUCCESS) {
				return false;
			}
		}
		else
			return false;

		return true;
	}

	void Sound::seekSound(float second)
	{
		if (loaded) {
			float myFFrame = static_cast<float>(m_pDecoder->outputSampleRate) * second;
			uint64_t myFrame = static_cast<uint64_t>(myFFrame);
			ma_decoder_seek_to_pcm_frame(m_pDecoder, myFrame);
		}
	}

	void Sound::setSoundVolume(float volume)
	{
		if (loaded) {
			ma_device_set_master_volume(m_pDevice, volume);
		}
	}




}