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

	public:
		bool			loaded;			// Sound Loaded
		std::string		filePath;		// file path
	private:
		ma_sound		m_sound;			// Internal miniaudio sound
		uint32_t		m_engineSampleRate;	// Sample rate we have defined in the engine
	};
}