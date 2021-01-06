#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <windows.h>

#include "GraphicsEngine.h"
#include "GameLogic.h"
#include "Sprites.h"

#define BOULDER_DENSITY 200 //im wieksza wartosc tym mniej
#define ALIEN_CHANCE 100



void detectCollisions(int xShipOld, int yShipOld, int*** boulderMap, struct game* gameData);

bool rmElement(struct dynamicLaserEntity** current);

void generateBoulders(int*** boulderMap, int width, int height);
void printBoulders(int** boulderMap, int width, int height);
void updateBoulders(int*** boulderMap, int xShipOld, int yShipOld, int width, int height);

void gameStats(struct game* gameData);
void finishGame(int width, int height);
void gameTick(unsigned long int* timeElapsed, unsigned long int* lastTime);

void generateAlien(int* alien, int* alienAI);
void updateAlienPosition(int* alien, int* alienAI, int xAlien, int yAlien, int* xAlienOld, int* yAlienOld, bool* alienState, int* alienDir, int width);

struct game {
	int score;
	int lives;
};

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
			if (frameDelayLaser > 300) {
				updateLaserPos(&current, &boulderMap, gameData);
				frameDelayLaser = 0;
			}
		}
		if (frameDelayBoulders > 1200) {
			updateBoulders(&boulderMap, xShipOld, yShipOld, width, height);
			printBoulders(boulderMap, width, height);
			if (alien != 0) {
				shootAlienLaser(&alienCurrent, xAlienOld, yAlienOld);
			}
			frameDelayBoulders = 0;
		}
		if (frameDelayAlien > 300) {
			updateAlienPosition(&alien, &alienAI, xAlien, yAlien, &xAlienOld, &yAlienOld, &alienState, &alienDir, width);
			if (alienCurrent != NULL) {
				updateAlienLaserPos(&alienCurrent, height);
			}
			frameDelayAlien = 0;
		}
	}
	return 0;
}

void gameTick(unsigned long int* timeElapsed, unsigned long int* lastTime) {
	unsigned long int currentTime = GetTickCount64();
	*timeElapsed = currentTime - *lastTime;
	*lastTime = currentTime;
}

bool rmElement(struct dynamicLaserEntity** current) {
	if (*current == NULL) return false;
	if (!((*current)->next) && !((*current)->previous)) {
		free(*current);
		*current = NULL;
		return true;
	}
	if ((*current)->next == NULL) {
		*current = (*current)->previous;
		free((*current)->next);
		(*current)->next = NULL;

		return true;
	}
	if ((*current)->previous == NULL) {
		*current = (*current)->next;
		free((*current)->previous);
		(*current)->previous = NULL;
		return true;
	}
	struct dynamicLaserEntity* copy = *current;
	*current = (*current)->previous;
	(*current)->next = (*current)->next->next;
	(*current)->next->previous = *current;
	free(copy);
	return true;
}

void generateBoulders(int*** boulderMap, int width, int height) {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			//5 = 200 / procent
			if ((rand() + 1) % BOULDER_DENSITY == 0) {
				(*boulderMap)[i][j] = 0;
			}
			if ((rand() + 1) % BOULDER_DENSITY == 1) {
				(*boulderMap)[i][j] = 1;
			}
		}
	}
}

void printBoulders(int** boulderMap, int width, int height) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (boulderMap[i][j] == 1) {
				setCoordinates(j, i);
				putchar('@');
			}
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void gameStats(struct game* gameData) {
	setCoordinates(10, 2);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	printf("SCORE: %d LIVES: %d", gameData->score, gameData->lives);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void detectCollisions(int xShipOld, int yShipOld, int*** boulderMap, struct game* gameData) {
	int xHitbox = xShipOld;
	int yHitbox = yShipOld;

	/* KOLIZJE Z GORY */

	if ((*boulderMap)[yHitbox][xHitbox] == 1) {
		gameData->lives--;
		(*boulderMap)[yHitbox][xHitbox] = 0;
	}
	if ((*boulderMap)[yHitbox][xHitbox + 1] == 1) {
		gameData->lives--;
		(*boulderMap)[yHitbox][xHitbox + 1] = 0;
	}
	if ((*boulderMap)[yHitbox][xHitbox + 2] == 1) {
		gameData->lives--;
		(*boulderMap)[yHitbox][xHitbox + 2] = 0;
	}
	if ((*boulderMap)[yHitbox][xHitbox + 3] == 1) {
		gameData->lives--;
		(*boulderMap)[yHitbox][xHitbox + 3] = 0;
	}
	if ((*boulderMap)[yHitbox][xHitbox + 4] == 1) {
		gameData->lives--;
		(*boulderMap)[yHitbox][xHitbox + 4] = 0;
	}

	/* KOLIZJE Z LEWEJ STRONY */

	if ((*boulderMap)[yHitbox + 1][xHitbox] == 1) {
		gameData->lives--;
		(*boulderMap)[yHitbox + 1][xHitbox] = 0;
	}
	if ((*boulderMap)[yHitbox + 2][xHitbox] == 1) {
		gameData->lives--;
		(*boulderMap)[yHitbox + 2][xHitbox] = 0;
	}

	/* KOLIZJE Z PRAWEJ STRONY */

	if ((*boulderMap)[yHitbox + 1][xHitbox + 4] == 1) {
		gameData->lives--;
		(*boulderMap)[yHitbox + 1][xHitbox + 4] = 0;
	}
	if ((*boulderMap)[yHitbox + 2][xHitbox + 4] == 1) {
		gameData->lives--;
		(*boulderMap)[yHitbox + 2][xHitbox + 4] = 0;
	}

	/* KOLIZJE Z DOLU */

	if ((*boulderMap)[yHitbox + 2][xHitbox + 1] == 1) {
		gameData->lives--;
		(*boulderMap)[yHitbox + 2][xHitbox + 1] = 0;
	}
	if ((*boulderMap)[yHitbox + 2][xHitbox + 2] == 1) {
		gameData->lives--;
		(*boulderMap)[yHitbox + 2][xHitbox + 2] = 0;
	}
	if ((*boulderMap)[yHitbox + 2][xHitbox + 3] == 1) {
		gameData->lives--;
		(*boulderMap)[yHitbox + 2][xHitbox + 3] = 0;
	}

}

void finishGame(int width, int height) {
	int x = (width - calcObjectWidth(death)) / 2;
	int y = height / 2;
	int temp1;
	int temp2;
	printObject(death, x, y, &temp1, &temp2);
}

void updateBoulders(int*** boulderMap, int xShipOld, int yShipOld, int width, int height) {
	/* WYCZYSZCZENIE */
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if ((*boulderMap)[i][j] == 1) {
				setCoordinates(j, i);
				putchar(' ');
			}
		}
	}
	int* temp = NULL;

	for (int i = height - 1; i > 0; i--) {
		temp = (*boulderMap)[i - 1];
		(*boulderMap)[i - 1] = (*boulderMap)[i];
		(*boulderMap)[i] = temp;
	}
	/* WYGENEROWANIE NOWEJ LINII */
	for (int j = 0; j < width; j++) {
		//20 = 200 / procent
		if ((rand() + 1) % BOULDER_DENSITY == 0) {
			(*boulderMap)[0][j] = 0;
		}
		if ((rand() + 1) % BOULDER_DENSITY == 1) {
			(*boulderMap)[0][j] = 1;
		}
	}
}

void generateAlien(int* alien, int* alienAI) {
	if ((rand() + 1) % ALIEN_CHANCE == 1) {
		*alien = rand() % (2 + 1 - 1) + 1;
		*alienAI = rand() % (2 + 1);
	}
}

void updateAlienPosition(int* alien, int* alienAI, int xAlien, int yAlien, int *xAlienOld, int *yAlienOld, bool* alienState, int* alienDir, int width) {
	switch (*alien) {
	case 1:
		clear(alienClear, *xAlienOld, *yAlienOld);
		xAlien = *xAlienOld;
		yAlien = *yAlienOld;
		if (*alienState == false) {
			printObject(alien1a, xAlien + *alienDir, yAlien, xAlienOld, yAlienOld);
		}
		else if(*alienState == true) {
			printObject(alien1b, xAlien + *alienDir, yAlien, xAlienOld, yAlienOld);
		}
		*alienState = !(*alienState);
		break;
	case 2:
		clear(alienClear, *xAlienOld, *yAlienOld);
		xAlien = *xAlienOld;
		yAlien = *yAlienOld;
		if (*alienState == false) {
			printObject(alien2a, xAlien + *alienDir, yAlien, xAlienOld, yAlienOld);
		}
		else if(*alienState == true) {
			printObject(alien2b, xAlien + *alienDir, yAlien, xAlienOld, yAlienOld);
		}
		*alienState = !(*alienState);
		break;
	case 0:
		return;
		break;
	}
	if (*xAlienOld == width - 10) {
		*alienDir = -(*alienDir);
	}
	if (*xAlienOld == 0) {
		*alien = 0;
	}


}