#ifndef Logic_h
#define Logic_h 

void shootLaser(struct dynamicLaserEntity** current, int xShipOld, int yShipOld);

void updateShipPos(int xShip, int yShip, int* xShipOld, int* yShipOld, struct dynamicLaserEntity** current, int** boulderMap, struct game* gameData);

void gameTick(unsigned long int* timeElapsed, unsigned long int* lastTime);

void rmElement(struct dynamicLaserEntity** current);

void updateLaserPos(struct dynamicLaserEntity** current, int*** boulderMap, struct game* gameData);

void generateBoulders(int*** boulderMap, int width, int height);

void printBoulders(int** boulderMap, int width, int height);

void detectCollisions(int xShipOld, int yShipOld, int*** boulderMap, struct game* gameData);

void updateBoulders(int*** boulderMap, int xShipOld, int yShipOld, int width, int height);

const unsigned char logo[];

const unsigned char death[];

const unsigned char ship[];

const unsigned char shipClear[];

const unsigned char laser[];

const unsigned char laserClear[];

#endif