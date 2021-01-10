#include <stdio.h>
#include <windows.h>
#include <stdbool.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include "GameLogic.h"
#include "Sprites.h"

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

struct alien {
	int presence;
	int x;
	int y;
	int xOld;
	int yOld;
	int AI;
};

struct ship {
	int x;
	int y;
};

void shootLaser(struct dynamicLaserEntity** current, int xShip, int yShip) {
	if (*current == NULL) {
		*current = malloc(sizeof(struct dynamicLaserEntity));
		if (*current != NULL) {
			(*current)->x = xShip + 2;
			(*current)->y = yShip - 1;
			(*current)->xOld = (*current)->x;
			(*current)->yOld = (*current)->y;
			(*current)->next = NULL;
			(*current)->previous = NULL;
			PlaySound(TEXT("laser.wav"), NULL, SND_ASYNC);
		}
	}
	else {
		struct dynamicLaserEntity* new = malloc(sizeof(struct dynamicLaserEntity));
		if (new != NULL) {
			new->x = xShip + 2;
			new->y = yShip - 1;
			new->xOld = new->x;
			new->yOld = new->y;
			(*current)->next = new;
			new->previous = *current;
			new->next = NULL;
			*current = new;
			PlaySound(TEXT("laser.wav"), NULL, SND_ASYNC);
		}
	}
}

void shootAlienLaser(struct dynamicLaserEntity** alienCurrent, struct alien alien) {
	if (*alienCurrent == NULL) {
		*alienCurrent = malloc(sizeof(struct dynamicLaserEntity));
		if (*alienCurrent != NULL) {
			(*alienCurrent)->x = alien.x + 2;
			(*alienCurrent)->y = alien.y + 2;
			(*alienCurrent)->xOld = (*alienCurrent)->x;
			(*alienCurrent)->yOld = (*alienCurrent)->y;
			(*alienCurrent)->next = NULL;
			(*alienCurrent)->previous = NULL;
		}
	}
	else {
		struct dynamicLaserEntity* new = malloc(sizeof(struct dynamicLaserEntity));
		if (new != NULL) {
			new->x = alien.x + 2;
			new->y = alien.y + 2;
			new->xOld = new->x;
			new->yOld = new->y;
			(*alienCurrent)->next = new;
			new->previous = *alienCurrent;
			new->next = NULL;
			*alienCurrent = new;
		}
	}
}

void updateLaserPos(struct dynamicLaserEntity** current, int*** boulderMap, struct game* gameData) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	if (*current != NULL) {
		while ((*current)->previous != NULL) { //przejscie na poczatek listy
			*current = (*current)->previous;
		}
		while (1) {
			if (*current == NULL) break;
			clear(laserClear, (*current)->xOld, (*current)->yOld);
			(*current)->x = (*current)->xOld;
			(*current)->y = (*current)->yOld;
			//setCoordinates((*current)->x, (*current)->y - 1);
			fflush(stdout);

			if ((*current)->yOld < 2) {
				if ((*current)->previous == NULL && (*current)->next == NULL) {
					free(*current);
					*current = NULL;
					break;
				}
				else {
					rmElement(current);
					//usun element ze srodka listy
				}
			}
			if ((*boulderMap)[(*current)->yOld][(*current)->xOld] == 1 || (*boulderMap)[(*current)->yOld][(*current)->xOld - 1] == 1 || (*boulderMap)[(*current)->yOld + 1][(*current)->xOld] == 1) {
				(*boulderMap)[(*current)->yOld][(*current)->xOld] = 0;
				rmElement(current);
				gameData->score += 25;
				if (*current == NULL) break;
			}
			printObject(laser, (*current)->x, (*current)->y - 1, &((*current)->xOld), &((*current)->yOld));
			if ((*current)->next != NULL) {
				*current = (*current)->next;
			}
			else {
				break;
			}
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void updateAlienLaserPos(struct dynamicLaserEntity** alienCurrent, int height, struct ship spaceship, struct game* gameData) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	if (*alienCurrent != NULL) {
		while ((*alienCurrent)->previous != NULL) { //przejscie na poczatek listy
			*alienCurrent = (*alienCurrent)->previous;
		}
		while (1) {
			if (*alienCurrent == NULL) break;
			clear(laserClear, (*alienCurrent)->xOld, (*alienCurrent)->yOld);
			(*alienCurrent)->x = (*alienCurrent)->xOld;
			(*alienCurrent)->y = (*alienCurrent)->yOld;
			//setCoordinates((*alienCurrent)->x, (*alienCurrent)->y - 1);
			fflush(stdout);

			if ((*alienCurrent)->yOld > height - 2) {
				if ((*alienCurrent)->previous == NULL && (*alienCurrent)->next == NULL) {
					free(*alienCurrent);
					*alienCurrent = NULL;
					break;
				}
				else {
					rmElement(alienCurrent);
					//usun element ze srodka listy
				}
			}
			printObject(laser, (*alienCurrent)->x, (*alienCurrent)->y + 1, &((*alienCurrent)->xOld), &((*alienCurrent)->yOld));
			if (((*alienCurrent)->yOld == spaceship.y && (*alienCurrent)->xOld == spaceship.x) ||
				((*alienCurrent)->yOld == spaceship.y && (*alienCurrent)->xOld == spaceship.x + 1) ||
				((*alienCurrent)->yOld == spaceship.y && (*alienCurrent)->xOld == spaceship.x + 2)
				)
			{
				rmElement(alienCurrent);
				gameData->lives = gameData->lives - ALIEN_DAMAGE;
			}
			if ((*alienCurrent)->next != NULL) {
				*alienCurrent = (*alienCurrent)->next;
			}
			else {
				break;
			}
		}
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void updateShipPos(struct ship* spaceship, struct dynamicLaserEntity** current, struct game* gameData) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	int input = getKeyboardInput();
	if (input == 1) {
		clear(shipClear, spaceship->x, spaceship->y);
		spaceship->x = spaceship->x - 1;
		printObject(ship, spaceship->x, spaceship->y, &(spaceship->x), &(spaceship->y));
	}
	if (input == 2) {
		clear(shipClear, spaceship->x, spaceship->y);
		spaceship->x = spaceship->x + 1;
		printObject(ship, spaceship->x, spaceship->y, &(spaceship->x), &(spaceship->y));
	}
	/*if (input == 3) {
		clear(shipClear, *xShipOld, *yShipOld);
		xShip = *xShipOld;
		yShip = *yShipOld;
		printObject(ship, xShip, yShip - 1, xShipOld, yShipOld);
	}
	if (input == 4) {
		clear(shipClear, *xShipOld, *yShipOld);
		xShip = *xShipOld;
		yShip = *yShipOld;
		printObject(ship, xShip, yShip + 1, xShipOld, yShipOld);
	}*/
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	if (input == 5) {
		//spacja
		shootLaser(current, spaceship->x, spaceship->y);
	}
}

void detectCollisions(struct ship spaceship, int*** boulderMap, struct game* gameData) {
	int xHitbox = spaceship.x;
	int yHitbox = spaceship.y;

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

void generateAlien(struct alien* alien) {
	if ((rand() % 100) < ALIEN_CHANCE) {
		alien->presence = rand() % (2 + 1 - 1) + 1;
		alien->AI = rand() % (2 + 1);
		PlaySound(TEXT("alien.wav"), NULL, SND_ASYNC);
	}
}

void updateAlienPosition(struct alien* alien, int* alienDir, int width) {
	static bool alienState = false;
	switch (alien->presence) {
	case 1:
		clear(alienClear, alien->xOld, alien->yOld);
		alien->x = alien->xOld;
		alien->y = alien->yOld;
		if (alienState == false) {
			printObject(alien1a, alien->x + *alienDir, alien->y, &(alien->xOld), &(alien->yOld));
		}
		else if (alienState == true) {
			printObject(alien1b, alien->x + *alienDir, alien->y, &(alien->xOld), &(alien->yOld));
		}
		alienState = !(alienState);
		break;
	case 2:
		clear(alienClear, alien->xOld, alien->yOld);
		alien->x = alien->xOld;
		alien->y = alien->yOld;
		if (alienState == false) {
			printObject(alien2a, alien->x + *alienDir, alien->y, &(alien->xOld), &(alien->yOld));
		}
		else if (alienState == true) {
			printObject(alien2b, alien->x + *alienDir, alien->y, &(alien->xOld), &(alien->yOld));
		}
		alienState = !alienState;
		break;
	case 0:
		return;
		break;
	}
	if (alien->xOld == width - 10) {
		*alienDir = -(*alienDir);
	}
	if (alien->xOld == 1 && *alienDir == -1) {
		alien->presence = 0;
		*alienDir = -(*alienDir);
		clear(alienClear, alien->xOld, alien->yOld);
	}
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

void updateBoulders(int*** boulderMap, int width, int height) {
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