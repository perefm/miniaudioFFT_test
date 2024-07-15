// Sound.h
// Spontz Demogroup

#pragma once 

#include "main.h"

#include <stdio.h>
#include <memory>
#include <string>
#include <string_view>

namespace Phoenix {

	class Sound;
	using SP_Sound = std::shared_ptr<Sound>;

	class Sound {
	public:
		Sound();
		virtual ~Sound();

	public:
		bool loadSoundFile(const std::string_view soundFile, ma_engine& engine); // Load sound from file
		bool playSound(); // Play Sound
		bool stopSound(); // Stop sound
		bool restartSound(); // Restart sound
		void seekSound(float second); // Seek sound
		void setSoundVolume(float volume);

	private:
		void unLoadSong();	// Unload song
		static void dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);


	public:
		bool			loaded;			// Sound Loaded
		std::string		filePath;		// file path
	private:
		ma_decoder		*m_pDecoder;	// Internal miniaudio decoder
		ma_device		*m_pDevice;		// Internal miniaudio device for playback
	};
}