#include <stdio.h>
#include <windows.h>

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

void shootAlienLaser(struct dynamicLaserEntity** alienCurrent, int xShipOld, int yShipOld) {
	if (*alienCurrent == NULL) {
		*alienCurrent = malloc(sizeof(struct dynamicLaserEntity));
		if (*alienCurrent != NULL) {
			(*alienCurrent)->x = xShipOld + 2;
			(*alienCurrent)->y = yShipOld + 2;
			(*alienCurrent)->xOld = (*alienCurrent)->x;
			(*alienCurrent)->yOld = (*alienCurrent)->y;
			(*alienCurrent)->next = NULL;
			(*alienCurrent)->previous = NULL;
		}
	}
	else {
		struct dynamicLaserEntity* new = malloc(sizeof(struct dynamicLaserEntity));
		if (new != NULL) {
			new->x = xShipOld + 2;
			new->y = yShipOld + 2;
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

void updateAlienLaserPos(struct dynamicLaserEntity** alienCurrent, int height) {
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