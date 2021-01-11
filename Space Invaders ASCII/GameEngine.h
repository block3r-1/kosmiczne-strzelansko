#ifndef GAMEENGINE_H
#define GAMEENGINE_H

void fullscreen();

void hideCursor();

void setCoordinates(int x, int y);

int calcObjectWidth(const unsigned char* object);

void getConsoleSize(int* width, int* height);

void printObject(const unsigned char* object, int x, int y);

void clear(const unsigned char* object, int x, int y);

void initializeConsole();

bool rmElement(struct dynamicLaserEntity** current);

void gameTick(unsigned long int* timeElapsed, unsigned long int* lastTime);

int getKeyboardInput();

void gameStats(struct game* gameData);
void finishGame(int width, int height);

#endif