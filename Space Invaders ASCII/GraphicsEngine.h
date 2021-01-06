#ifndef GRAPHICSENGINE_H
#define GRAPHICSENGINE_H

void fullscreen();

void hideCursor();

void setCoordinates(int x, int y);

int calcObjectWidth(const unsigned char* object);

void getConsoleSize(int* width, int* height);

void printObject(const unsigned char* object, int x, int y, int* xOld, int* yOld);

void clear(const unsigned char* object, int x, int y);

void initializeConsole();

#endif