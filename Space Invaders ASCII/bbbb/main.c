#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <stdbool.h>

#include "Engine.h"
#include "Logic.h"

struct pos {
	int x;
	int y;
	int xOld;
	int yOld;
};

struct dynamicLaserEntity {
	struct pos;
	struct dynamicLaserEntity* next;
	struct dynamicLaserEntity* previous;
};

struct game {
	int score;
	int lives;
};

void gameStats(struct game* gameData);
void finishGame(int width, int height);

int main() {
	srand(time(NULL));
	int width = 0;
	int height = 0;
	int xShip = 0;
	int yShip = 0;
	int xShipOld = 0;
	int yShipOld = 0;
	unsigned long int timeElapsed = 0;
	unsigned long int lastTime = 0;
	int frameDelayLaser = 0;
	int frameDelayBoulders = 0;
	struct dynamicLaserEntity* current = NULL;
	struct game* gameData = malloc(sizeof(struct game));
	if (gameData != NULL) {
		gameData->lives = 2137;
		gameData->score = 0;
	}
	else {
		printf("Nie udalo sie zaalokowac pamieci! \n");
		exit(-1);
	}
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

		updateShipPos(xShip, yShip, &xShipOld, &yShipOld, &current, boulderMap, gameData);
		detectCollisions(xShipOld, yShipOld, &boulderMap, gameData);
		gameStats(gameData);

		if (gameData->lives == 0) {
			finishGame(width, height);
			quit = true;
		}

		if(current != NULL) {
			if (frameDelayLaser > 300) {
				updateLaserPos(&current, &boulderMap, gameData);
				frameDelayLaser = 0;
			}
		}
		if (frameDelayBoulders > 1200) {
			updateBoulders(&boulderMap, xShipOld, yShipOld, width, height);
			printBoulders(boulderMap, width, height);
			frameDelayBoulders = 0;
		}
	}
	return 0;
}



void gameStats(struct game* gameData) {
	setCoordinates(10, 2);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	printf("SCORE: %d LIVES: %d", gameData->score, gameData->lives);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}



void finishGame(int width, int height) {
	int x = (width - calcObjectWidth(death)) / 2;
	int y = height / 2;
	int temp1;
	int temp2;
	printObject(death, x, y, &temp1, &temp2);
}
