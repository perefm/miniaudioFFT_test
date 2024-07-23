
#include <stdio.h>
#include <conio.h>
#include <memory>
#include <string>
#include <string_view>
#include <chrono>


#define PHOENIX_MAIN
#include "main.h"

#include "sound/SoundManager.h"

using namespace Phoenix;


void playSound(SoundManager &sm, uint32_t id) {
	SP_Sound mySound;
	mySound = sm.getSoundbyID(id);
	if (mySound) {
		if (!mySound->playSound())
			printf("\nError playing Sound %d", id);
		printf("\nPlaying Sound %d - %s", id, mySound->filePath.c_str());
	}
}

void restartSound(SoundManager& sm, uint32_t id) {
	SP_Sound mySound;
	mySound = sm.getSoundbyID(id);
	if (mySound) {
		if (!mySound->restartSound())
			printf("\nError restarting Sound %d", id);
		printf("\nRestarted Sound %d - %s", id, mySound->filePath.c_str());
	}
}

void stopSound(SoundManager& sm, uint32_t id) {
	SP_Sound mySound;
	mySound = sm.getSoundbyID(id);
	if (mySound) {
		if (!mySound->stopSound())
			printf("\nError stopping Sound %d", id);
		printf("\nStopped Sound %d - %s", id, mySound->filePath.c_str());
	}
}

void seekSound(SoundManager& sm, uint32_t id, float second) {
	SP_Sound mySound;
	mySound = sm.getSoundbyID(id);
	if (mySound) {
		mySound->seekSound(second);
		printf("\nMoving to second %.2f on Sound %d - %s", second, id, mySound->filePath.c_str());
	}
}

void setSoundVolume(SoundManager& sm, uint32_t id, float volume) {
	SP_Sound mySound;
	mySound = sm.getSoundbyID(id);
	if (mySound) {
		mySound->volume = volume;
		printf("\nSet volume %.2f on Sound %d - %s", volume, id, mySound->filePath.c_str());
	}
}

void fftAnalysis(SoundManager& sm) {
			
	std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();


	while (!_kbhit()) {
		start_time = std::chrono::steady_clock::now();
		std::chrono::duration<float> frame_time = start_time - end_time;

		sm.performFFT(frame_time.count());

		end_time = std::chrono::steady_clock::now();

		printf("\n");
		
		uint32_t bars = 32;
		uint32_t blocks = FFT_SIZE / bars; // blocks inside spectrum
		float value = 0;
		
		/*
		// Histogram
		for (uint32_t i = 0; i < blocks; i++) {

			float value = 0;
			for (uint32_t j = 0; j < bars; j++) {
				value += sm.m_pFFTBuffer[i * bars + j]; // we accumulate values for the same bar
			}
			printf("%.1f ", value); // Print the value of the block
		}
		*/

		// L/M/H value frequencies
		//printf("%.1f - %.1f - %.1f", sm.m_lowFreqSum, sm.m_midFreqSum, sm.m_highFreqSum); // Print the value of the Frequencies analyzed

		// Beat detection
		printf("Beat: %.5f", sm.m_fBeat); // Print the beat value
	}
}


int main(int argc, char* argv[])
{
	SoundManager soundManager;

	char charCaptured = 0;
	printf("\nMiniaudio version: %s\n", soundManager.getVersion().c_str());

	soundManager.enumerateDevices();

	printf("\nPress 'z' to quit...\n");
	printf("\n1-Load song: \"piano.mp3\"");
	printf("\n2-Load song: \"1-20kHz.wav\"");
	printf("\nq-Play song 0");
	printf("\nw-Play song 1");
	printf("\na-Stop song 0");
	printf("\ns-Stop song 1");
	printf("\nr-Reset song 0");
	printf("\nt-Reset song 1");
	printf("\nf-Jump to second 10 in song 0");
	printf("\ng-Jump to second 10 in song 1");

	printf("\n\n8-Master Volume at 0%%");
	printf("\n9-Master Volume at 50%%");
	printf("\n0-Master Volume at 100%%");

	printf("\n\nu-Song 0 Volume at 0%%");
	printf("\ni-Song 0 Volume at 50%%");
	printf("\no-Song 0 Volume at 100%%");

	printf("\n7- Show FFT analysis");

	printf("\n\np-Clear all songs from memory");
	
	printf("\n\nChoose wisely!!!!\n");

	soundManager.playDevice();

	while (charCaptured != 'z') {
		charCaptured = getchar();
		switch (charCaptured) {
		case '1':
			if (!soundManager.addSound("files/music.mp3"))
				printf("\nError loading Song");
			else {
				int slot = static_cast<int>(soundManager.sound.size()) - 1;
				printf("\nLoaded Song %s in slot %d", soundManager.sound[slot]->filePath.c_str(), slot);
			}
				
			break;
		case '2':
			if (!soundManager.addSound("files/1-20kHz.wav"))
				printf("\nError loading Song");
			else {
				int slot = static_cast<int>(soundManager.sound.size()) - 1;
				printf("\nLoaded Song %s in slot %d", soundManager.sound[slot]->filePath.c_str(), slot);
			}
			break;
		case 'q':
			playSound(soundManager, 0);
			break;
		case 'w':
			playSound(soundManager, 1);
			break;
		case 'r':
			restartSound(soundManager, 0);
			break;
		case 't':
			restartSound(soundManager, 1);
			break;
		case 'a':
			stopSound(soundManager, 0);
			break;
		case 's':
			stopSound(soundManager, 1);
			break;
		case 'f':
			seekSound(soundManager, 0, 10.0f);
			break;
		case 'g':
			seekSound(soundManager, 1, 10.0f);
			break;

		case '7':
			fftAnalysis(soundManager);
			break;

		// Master volume
		case '8':
			if (soundManager.setMasterVolume(0.0f))
				printf("\nVolume at 0%%");
			break;
		case '9':
			if (soundManager.setMasterVolume(0.5f))
				printf("\nVolume at 50%%");
			break;
		case '0':
			if (soundManager.setMasterVolume(1.0f))
				printf("\nVolume at 100%%");
			break;
		
		// individual song volume
		case 'u':
			setSoundVolume(soundManager, 0, 0.0f);
			break;
		case 'i':
			setSoundVolume(soundManager, 0, 0.5f);
			break;

		case 'o':
			setSoundVolume(soundManager, 0, 1.0f);
			break;

		case 'p':
			printf("\nCleared all songs");
			soundManager.clearSounds();
			break;

		case 'z':
			printf("\nBye!");

			soundManager.stopDevice();
			soundManager.clearSounds();
			break;
		}
	}



	return 0;
}