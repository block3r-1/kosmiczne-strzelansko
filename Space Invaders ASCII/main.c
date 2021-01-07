#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <windows.h>

#include "GameEngine.h"
#include "GameLogic.h"
#include "Sprites.h"

#define TIME_BOULDER 1200 //ms
#define TIME_LASER 300
#define TIME_ALIEN 300

struct game {
	int score;
	int lives;
};

int main() {
	srand(time(NULL));
	int width = 0;
	int height = 0;

	int xShip = 0;
	int yShip = 0;
	int xShipOld = 0;
	int yShipOld = 0;

	int xAlien = 0;
	int yAlien = 20;
	int xAlienOld = 0;
	int yAlienOld = 20;

	unsigned long int timeElapsed = 0;
	unsigned long int lastTime = 0;
	int frameDelayLaser = 0;
	int frameDelayBoulders = 0;
	int frameDelayAlien = 0;
	int frameDelayAlienLaser = 0;

	struct dynamicLaserEntity* current = NULL;
	struct dynamicLaserEntity* alienCurrent = NULL;

	struct game* gameData = malloc(sizeof(struct game));
	if (gameData != NULL) {
		gameData->lives = 2137;
		gameData->score = 0;
	}
	else {
		printf("Nie udalo sie zaalokowac pamieci! \n");
		exit(-1);
	}
	int alien = 0;
	int alienAI = 0;
	int alienDir = 1;

	bool alienState = false;
	bool quit = false;

	initializeConsole();
	getConsoleSize(&width, &height);
	xShip = width / 2;
	yShip = height - 5;
	int** boulderMap = NULL;
	if (height > 0 && width > 0) {
		boulderMap = malloc(height * sizeof(int*));
		if (boulderMap != NULL) {
			for (int i = 0; i < height; i++) {
				boulderMap[i] = malloc(width * sizeof(int));
				if (boulderMap[i] == NULL) {
					printf("Blad! Nie mozna zaalokowac pamieci! \n");
					for (int j = 0; j < i; j++) {
						free(boulderMap[j]);
					}
					exit(-1);
				}
			}
		}
		else {
			printf("Blad! Nie mozna zaalokowac pamieci! \n");
			exit(-1);
		}
	}
	else {
		printf("Cos poszlo nie tak! Nie mozna odczytac wymiarow konsoli!");
		exit(-1);
	}
	generateBoulders(&boulderMap, width, height);
	printBoulders(boulderMap, width, height);
	lastTime = GetTickCount64();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	printObject(ship, xShip, yShip, &xShipOld, &yShipOld);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	while (quit == false) {
	// glowna petla gry
		gameTick(&timeElapsed, &lastTime);
		frameDelayLaser += timeElapsed;
		frameDelayBoulders += timeElapsed;
		frameDelayAlien += timeElapsed;

		updateShipPos(xShip, yShip, &xShipOld, &yShipOld, &current, boulderMap, gameData);
		detectCollisions(xShipOld, yShipOld, &boulderMap, gameData);
		if (alien == 0) {
			generateAlien(&alien, &alienAI);
		}
		gameStats(gameData);

		if (gameData->lives == 0) {
			finishGame(width, height);
			quit = true;
		}

		if(current != NULL) {
			if (frameDelayLaser > TIME_LASER) {
				updateLaserPos(&current, &boulderMap, gameData);
				frameDelayLaser = 0;
			}
		}
		if (frameDelayBoulders > TIME_BOULDER) {
			updateBoulders(&boulderMap, xShipOld, yShipOld, width, height);
			printBoulders(boulderMap, width, height);
			if (alien != 0) {
				shootAlienLaser(&alienCurrent, xAlienOld, yAlienOld);
			}
			frameDelayBoulders = 0;
		}
		if (frameDelayAlien > TIME_ALIEN) {
			updateAlienPosition(&alien, &alienAI, xAlien, yAlien, &xAlienOld, &yAlienOld, &alienState, &alienDir, width);
			if (alienCurrent != NULL) {
				updateAlienLaserPos(&alienCurrent, height, xShipOld, yShipOld, gameData);
			}
			frameDelayAlien = 0;
		}
	}
	return 0;
}