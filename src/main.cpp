
#include <stdio.h>
#include <conio.h>
#include <memory>
#include <string>
#include <string_view>

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

void fftAnalysis(SoundManager& sm) {
			
	while (!_kbhit()) {
		sm.performFFT();
		printf("\n");
		/*
		uint32_t bars = 32;
		uint32_t blocks = FFT_SIZE / bars; // blocks inside spectrum
		float value = 0;

		for (uint32_t i = 0; i < blocks; i++) {
			float value = 0;
			for (uint32_t j = 0; j < bars; j++) {
				value += sm.m_fftBuffer[i * bars + j]; // we accumulate values for the same bar
			}
			printf("%.1f ", value); // Print the value of the block
		}
		*/
		printf("%.1f - %.1f - %.1f", sm.m_lowFreqSum, sm.m_midFreqSum, sm.m_highFreqSum); // Print the value of the Frequencies analyzed
	}
}


int main(int argc, char* argv[])
{
	SoundManager soundManager;

	char charCaptured = 0;
	printf("\nMiniaudio version: %s\n", soundManager.getVersion().c_str());

	soundManager.enumerateDevices();

	printf("\nPress 'z' to quit...\n");
	printf("\n1-Load song \"piano.mp3\"");
	printf("\n2-Load song \"2.5K-100hz.mp3\"");
	printf("\nq-Play song 0");
	printf("\nw-Play song 1");
	printf("\na-Stop song 0");
	printf("\ns-Stop song 1");
	printf("\nr-Reset song 0");
	printf("\nt-Reset song 1");
	printf("\nf-Jump to second 10 in song 0");
	printf("\ng-Jump to second 10 in song 1");

	printf("\n\n8-Volume at 0%%");
	printf("\n9-Volume at 50%%");
	printf("\n0-Volume at 100%%");

	printf("\n7- Show FFT analysis");

	printf("\n\np-Clear all songs from memory");
	
	printf("\n\nChoose wisely!!!!\n");

	soundManager.playDevice();

	while (charCaptured != 'z') {
		charCaptured = getchar();
		switch (charCaptured) {
		case '1':
			if (nullptr == soundManager.addSound("files/piano.mp3"))
				printf("\nError loading Song 1");
			printf("\nLoaded Song 1");
			break;
		case '2':
			if (nullptr == soundManager.addSound("files/2.5K-100hz.mp3"))
					printf("\nError loading Song 2");
			printf("\nLoaded Song 2");
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

		case '8':
			if (soundManager.setVolume(0.0f))
				printf("\nVolume at 0%%");
			break;
		case '9':
			if (soundManager.setVolume(0.5f))
				printf("\nVolume at 50%%");
			break;

		case '0':
			if (soundManager.setVolume(1.0f))
				printf("\nVolume at 100%%");
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