﻿#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <windows.h>

#include "GameEngine.h"
#include "GameLogic.h"
#include "Sprites.h"

#define TIME_BOULDER 750 //ms
#define TIME_LASER 100
#define TIME_LASER_SHOOT 350
#define TIME_ALIEN 100
#define TIME_ALIEN_GENERATION 1000
#define TIME_ALIEN_LASER 100

struct game {
	int score;
	int lives;
	unsigned long int time;
};

struct alien {
	int presence;
	int x;
	int y;
	int AI;
	int type;
	int dir;
};

struct ship {
	int x;
	int y;
	bool laserShot;
};

int main() {
	srand(time(NULL));
	int width = 0;
	int height = 0;

	unsigned long int timeElapsed = 0;
	unsigned long int lastTime = 0;
	int frameDelayLaser = 0;
	int frameDelayLaserShoot = 0;
	int frameDelayBoulders = 0;
	int frameDelayAlien = 0;
	int frameDelayAlienGeneration = 0;
	int frameDelayAlienLaser = 0;

	struct dynamicLaserEntity* current = NULL;
	struct dynamicLaserEntity* alienCurrent = NULL;
	struct alien alien;
	struct ship spaceship;

	alien.presence = 0;
	alien.x = 0;
	alien.y = 20;
	alien.AI = 0;

	spaceship.x = 0;
	spaceship.y = 0;
	spaceship.laserShot = false;

	struct game* gameData = malloc(sizeof(struct game));
	if (gameData != NULL) {
		gameData->lives = 20;
		gameData->score = 0;
		gameData->time = 0;
	}
	else {
		printf("Nie udalo sie zaalokowac pamieci! \n");
		exit(-1);
	}

	int alienDir = 1;

	bool quit = false;

	initializeConsole();
	getConsoleSize(&width, &height);

	spaceship.x = width / 2;
	spaceship.y = height - 5;

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

	//printObject(logo, (width - calcObjectWidth(logo)) / 2, height / 2);
//	Sleep(5000);
	system("cls");

	generateBoulders(&boulderMap, width, height);
	printBoulders(boulderMap, width, height);

	lastTime = GetTickCount64();

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	printObject(ship, spaceship.x, spaceship.y);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

	int	check = 0;

	while (quit == false) {
		fflush(stdout);
	// glowna petla gry
		gameTick(&timeElapsed, &lastTime);
		frameDelayLaser += timeElapsed;
		frameDelayLaserShoot += timeElapsed;
		frameDelayBoulders += timeElapsed;
		frameDelayAlien += timeElapsed;
		frameDelayAlienGeneration += timeElapsed;
		frameDelayAlienLaser += timeElapsed;
		gameData->time += timeElapsed;

		check = updateShipPos(&spaceship, &current, boulderMap, gameData);
		if (check == -1) {
			quit = true;
		}
		detectCollisions(spaceship, &boulderMap, gameData);
		if (alien.presence == 0) {
			if (frameDelayAlienGeneration > TIME_ALIEN_GENERATION) {
				generateAlien(&alien);
				frameDelayAlienGeneration = 0;
			}
		}
		gameStats(gameData);

		if (gameData->lives < 1) {
			finishGame(width, height);
			Sleep(4000);
			quit = true;
		}

		if (frameDelayBoulders > TIME_BOULDER) {
			updateBoulders(&boulderMap, width, height);
			printBoulders(boulderMap, width, height);
			frameDelayBoulders = 0;
		}
		if (frameDelayLaserShoot > TIME_LASER_SHOOT) {
			if (spaceship.laserShot == true) {
				shootLaser(&current, spaceship.x, spaceship.y);
				spaceship.laserShot = false;
				frameDelayLaserShoot = 0;
			}
		}

		if (current != NULL) {
			if (frameDelayLaser > TIME_LASER) {
				updateLaserPos(&current, &boulderMap, gameData);
				frameDelayLaser = 0;
			}
		}

		if (frameDelayAlien > TIME_ALIEN) {
			updateAlienPosition(&alien, &alienDir, width);

			if (alien.presence == 1) {
				if ((rand() % 100) < ALIEN_LASER_CHANCE) {
					shootAlienLaser(&alienCurrent, alien);
				}
			}

			if (alienCurrent != NULL) {
				if (frameDelayAlienLaser > TIME_ALIEN_LASER) {
					updateAlienLaserPos(&alienCurrent, height, spaceship, gameData);
					frameDelayAlienLaser = 0;
				}
			}
			frameDelayAlien = 0;
		}
	}
	/* PROCEDURA ZAMKNIECIA GRY */
	freeMemory(&current, &alienCurrent, &boulderMap, height);
	saveStatsToFile(gameData);
	free(gameData);
	return 0;
}