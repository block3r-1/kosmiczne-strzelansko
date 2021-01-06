#include <stdio.h>
#include <stdlib.h>
#include <windows.h>


#define BOULDER_DENSITY 200 //im wieksza wartosc tym mniej

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

void shootLaser(struct dynamicLaserEntity** current, int xShipOld, int yShipOld) {
	if (*current == NULL) {
		*current = malloc(sizeof(struct dynamicLaserEntity));
		if (*current != NULL) {
			(*current)->x = xShipOld + 2;
			(*current)->y = yShipOld - 2;
			(*current)->xOld = (*current)->x;
			(*current)->yOld = (*current)->y;
			(*current)->next = NULL;
			(*current)->previous = NULL;
		}
	}
	else {
		struct dynamicLaserEntity* new = malloc(sizeof(struct dynamicLaserEntity));
		if (new != NULL) {
			new->x = xShipOld + 2;
			new->y = yShipOld - 1;
			new->xOld = new->x;
			new->yOld = new->y;
			(*current)->next = new;
			new->previous = *current;
			new->next = NULL;
			*current = new;
		}
	}
}

void updateShipPos(int xShip, int yShip, int* xShipOld, int* yShipOld, struct dynamicLaserEntity** current, int** boulderMap, struct game* gameData) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	int input = getKeyboardInput();
	if (input == 1) {
		clear(shipClear, *xShipOld, *yShipOld);
		xShip = *xShipOld;
		yShip = *yShipOld;
		printObject(ship, xShip - 1, yShip, xShipOld, yShipOld);
	}
	if (input == 2) {
		clear(shipClear, *xShipOld, *yShipOld);
		xShip = *xShipOld;
		yShip = *yShipOld;
		printObject(ship, xShip + 1, yShip, xShipOld, yShipOld);
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
		shootLaser(current, *xShipOld, *yShipOld);
	}
}

void gameTick(unsigned long int* timeElapsed, unsigned long int* lastTime) {
	unsigned long int currentTime = GetTickCount64();
	*timeElapsed = currentTime - *lastTime;
	*lastTime = currentTime;
}

void rmElement(struct dynamicLaserEntity** current) {
	if (*current == NULL) return;
	if (!((*current)->next) && !((*current)->previous)) {
		free(*current);
		*current = NULL;
		return;
	}
	if ((*current)->next == NULL) {
		*current = (*current)->previous;
		free((*current)->next);
		(*current)->next = NULL;

		return;
	}
	if ((*current)->previous == NULL) {
		*current = (*current)->next;
		free((*current)->previous);
		(*current)->previous = NULL;
		return;
	}
	struct dynamicLaserEntity* copy = *current;
	*current = (*current)->previous;
	(*current)->next = (*current)->next->next;
	(*current)->next->previous = *current;
	free(copy);
	return;
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

