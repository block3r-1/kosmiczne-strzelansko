#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <stdbool.h>
#include <errno.h>

#include "Sprites.h"

struct game {
	int score;
	int lives;
	unsigned long int time;
};

struct pos {
	int x;
	int y;
};

struct dynamicLaserEntity {
	struct pos;
	struct dynamicLaserEntity* next;
	struct dynamicLaserEntity* previous;
};

void fullscreen() {
	/*keybd_event(VK_MENU, 0x38, 0, 0);
	keybd_event(VK_RETURN, 0x1c, 0, 0);
	keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
	*/
	HWND handle = GetConsoleWindow();
	ShowWindow(handle, SW_SHOWMAXIMIZED);
}

void hideCursor()
{
	CONSOLE_CURSOR_INFO cursor;
	cursor.dwSize = 100;
	cursor.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
}

void setCoordinates(int x, int y) {
	COORD cursorPos;
	cursorPos.X = x;
	cursorPos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPos);
}

int calcObjectWidth(const unsigned char* object) {
	int size = 0;
	int i = 0;
	while (object[i] != '\n') {
		size++;
		i++;
	} size++;
	return size;
}

void getConsoleSize(int* width, int* height) {
	//int size = calcObjectWidth(object);
	COORD screenData = GetLargestConsoleWindowSize(GetStdHandle(STD_OUTPUT_HANDLE));
	*width = screenData.X;
	*height = screenData.Y;
	//COORD consoleBufferSize = { *width, *height };
	//SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), consoleBufferSize);
	//*x = (width - size) / 2;
	//*y = height / 2;
}

void printObject(const unsigned char* object, int x, int y) {
	int i = 0;
	setCoordinates(x, y);
	while (object[i] != '\0') {
		putchar(object[i]);
		fflush(stdout);
		if (object[i] == '\n') {
			y++;
			setCoordinates(x, y);
		}
		i++;
	}
}

void clear(const unsigned char* object, int x, int y)
{
	setCoordinates(x, y);
	int i = 0;
	while (object[i] != '\0') {
		char r = object[i];
		putchar(object[i]);
		fflush(stdout);
		if (object[i] == '\n') {
			y++;
			setCoordinates(x, y);
		}
		i++;
	}
}

void initializeConsole() {
	fullscreen();
	hideCursor();
	//centerConsoleBuffer(ship, x, y);
}

int getKeyboardInput() {
	if (_kbhit()) {
		switch (_getch()) {

			/*case 'w':
				//strzalka w gore
				return 3;
				break;
			case 's':
				//strzalka w dol
				return 4;
				break;
				*/

		case 'a':
			//strzalka w lewo
			return 1;
			break;
		case 'd':
			//strzalka w prawo
			return 2;
			break;
		case 'f':
			//spacja
			return 5;
			break;
		case 'q':
			return 6;
			break;
		default:
			return 0;
			break;
		}
	}
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

void gameStats(struct game* gameData) {
	setCoordinates(10, 2);
	char score[] = "SCORE: \0";
	char lives[] = "LIVES: \0";
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

void freeMemory(struct dynamicLaserEntity** current, struct dynamicLaserEntity** alienCurrent, int*** boulderMap, int height) {
	if (*current != NULL) {
		while ((*current)->previous != NULL) { //przejscie na poczatek listy
			*current = (*current)->previous;
		}
		while ((*current)->next != NULL && (*current)->previous != NULL) {
			*current = (*current)->next;
			free((*current)->previous);
			(*current)->previous = NULL;
		} free(*current);
	}
	if (*alienCurrent != NULL) {
		while ((*alienCurrent)->previous != NULL) { //przejscie na poczatek listy
			*alienCurrent = (*alienCurrent)->previous;
		}
		while ((*alienCurrent)->next != NULL && (*alienCurrent)->previous != NULL) {
			*alienCurrent = (*alienCurrent)->next;
			free((*alienCurrent)->previous);
			(*alienCurrent)->previous = NULL;
		} free(*alienCurrent);
	}
	for (int i = 0; i < height; i++) {
		free((*boulderMap)[i]);
	} free(*boulderMap);
}

void saveStatsToFile(struct game* gameData) {
	FILE* file = fopen("stats.txt", "w");
	if (file == NULL) {
		perror("Stop! Blad otwarcia pliku");
		return;
	}
	fprintf(file, "Lives: %d \n", gameData->lives);
	fprintf(file, "Score: %d \n", gameData->score);
	fprintf(file, "Boulders destroyed: %d \n", gameData->score / 25);
	fprintf(file, "Game time: %d seconds \n", gameData->time / 1000);
}