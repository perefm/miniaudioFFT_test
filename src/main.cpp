
#include <stdio.h>
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

int main(int argc, char* argv[])
{
	SoundManager soundManager;

	if (!soundManager.initEngine())
		printf("\nError at Init!\n");

	char charCaptured = 0;
	printf("\nMiniaudio version: %s\n", soundManager.getVersion().c_str());

	soundManager.enumerateDevices();

	printf("\nPress 'z' to quit...\n");
	printf("\n1-Load song 1");
	printf("\n2-Load song 2");
	printf("\nq-Play song 1");
	printf("\nw-Play song 2");
	printf("\na-Stop song 1");
	printf("\ns-Stop song 2");
	printf("\nr-Reset song 1");
	printf("\nt-Reset song 2");
	printf("\nf-Jump to second 10 in song 1");
	printf("\ng-Jump to second 10 in song 2");

	printf("\n\n8-Volume at 0%%");
	printf("\n9-Volume at 50%%");
	printf("\n0-Volume at 100%%");

	printf("\n\np-Clear all songs from memory");
	
	printf("\n\nChoose wisely!!!!\n");

	while (charCaptured != 'z') {
		charCaptured = getchar();
		switch (charCaptured) {
		case '1':
			if (nullptr == soundManager.addSound("files/piano.mp3"))
				printf("\nError loading Song 1");
			printf("\nLoaded Song 1");
			break;
		case '2':
			if (nullptr == soundManager.addSound("files/Low.mp3"))
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

			soundManager.clearSounds();
			soundManager.uninitEngine();
			break;
		}
	}



	return 0;
}