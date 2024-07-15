// Sound.cpp
// Spontz Demogroup

#include "main.h"
#include "sound/Sound.h"

namespace Phoenix {

	Sound::Sound()
		:
		m_engineSampleRate(0),
		filePath(""),
		loaded(false)
	{
	}

	Sound::~Sound()
	{
		if (loaded) {
			ma_sound_stop(&m_sound);
			ma_sound_uninit(&m_sound);
			loaded = false;
		}
	}

	bool Sound::loadSoundFile(const std::string_view soundFile, ma_engine& engine)
	{
		ma_result result;

		// Re-load the file if needed
		if (loaded) {
			ma_sound_uninit(&m_sound);
			loaded = false;
		}
		filePath = soundFile;
		// Disable sound Spatialization to save some resources, because we dont need it
		result = ma_sound_init_from_file(&engine, soundFile.data(), MA_SOUND_FLAG_NO_SPATIALIZATION, NULL, NULL, &m_sound);
		if (result != MA_SUCCESS) {
			loaded = false;
		}
		else {
			m_engineSampleRate = ma_engine_get_sample_rate(&engine);
			loaded = true;
		}
		
		return loaded;
	}

	bool Sound::playSound()
	{
		ma_result result;
		if (loaded) {
			result = ma_sound_start(&m_sound);
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
			result = ma_sound_stop(&m_sound);
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
			result = ma_sound_seek_to_pcm_frame(&m_sound,0);
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
			float myFFrame = static_cast<float>(m_engineSampleRate) * second;
			uint64_t myFrame = static_cast<uint64_t>(myFFrame);
			ma_sound_seek_to_pcm_frame(&m_sound, myFrame);
			//ma_sound_set_start_time_in_milliseconds(&m_sound, millisec);
		}
	}

	void Sound::setSoundVolume(float volume)
	{
		if (loaded) {
			ma_sound_set_volume(&m_sound, volume);
		}
	}


}