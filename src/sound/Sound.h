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
		enum State {
			NotReady = 0,
			Playing,
			Stopped,
			Finished,
		};


	public:
		Sound();
		virtual ~Sound();

	public:
		bool loadSoundFile(const std::string_view soundFile, uint32_t channels, uint32_t sampleRate); // Load sound from file
		bool playSound(); // Play Sound
		bool stopSound(); // Stop sound
		bool restartSound(); // Restart sound
		void seekSound(float second); // Seek sound
		ma_decoder* getDecoder();

	private:
		void unLoadSong();	// Unload song

	public:
		std::string		filePath;		// file path
		Sound::State	status;			// Sound status

	private:
		ma_decoder		*m_pDecoder;	// Internal miniaudio decoder

	};
}