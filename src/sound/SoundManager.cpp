// SoundManager.cpp
// Spontz Demogroup

#include "main.h"
#include "sound/SoundManager.h"

namespace Phoenix {
	SoundManager::SoundManager()
	{
		ma_result result;


		sound.clear();
		m_LoadedSounds = 0;
		m_inited = false;

		// Resource manager initialization
		m_resourceManagerConfig = ma_resource_manager_config_init();
		m_resourceManagerConfig.decodedFormat = ma_format_f32;	// ma_format_f32 should almost always be used as that's what the engine (and most everything else) uses for mixing
		m_resourceManagerConfig.decodedChannels = 0;			// Setting the channel count to 0 will cause sounds to use their native channel count
		m_resourceManagerConfig.decodedSampleRate = 44100;		// Using a consistent sample rate is useful for avoiding expensive resampling in the audio thread. This will result in resampling being performed by the loading thread(s)

		result = ma_resource_manager_init(&m_resourceManagerConfig, &m_resourceManager);
		if (result != MA_SUCCESS) {
			printf("\nFailed to initialize the resource manager.");
		}
	}

	SoundManager::~SoundManager()
	{
		clearSounds();
		ma_resource_manager_uninit(&m_resourceManager);
	}
	
	bool SoundManager::initEngine()
	{
		if (!m_inited) {
			ma_result result;
			// Sound engine Init
			result = ma_engine_init(NULL, &m_engine); // TODO: Can we init engione before or after the Resource manager? or is not even needed?
			if (result != MA_SUCCESS) {
				m_inited = false;
			}
			else
				m_inited = true;
		}
		return m_inited;
	}

	void SoundManager::uninitEngine()
	{
		ma_engine_uninit(&m_engine);
	}

	bool SoundManager::setVolume(float volume)
	{
		if (m_inited) {
			ma_result result;
			result = ma_engine_set_volume(&m_engine, volume);
			if (result != MA_SUCCESS)
				return false;
		}
		return true;
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
			if (new_sound->loadSoundFile(filePath, m_engine)) {
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

/*	void SoundManager::dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
	{
		ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
		if (pDecoder == NULL) {
			return;
		}

		ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

		(void)pInput;
	}
*/
}
