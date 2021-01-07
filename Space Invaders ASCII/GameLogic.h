#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#define BOULDER_DENSITY 200 //im wieksza wartosc tym mniej
#define ALIEN_CHANCE 100
#define ALIEN_DAMAGE 10

void shootLaser(struct dynamicLaserEntity** current, int xShipOld, int yShipOld);
void shootAlienLaser(struct dynamicLaserEntity** alienCurrent, int xShipOld, int yShipOld);

void updateShipPos(int xShip, int yShip, int* xShipOld, int* yShipOld, struct dynamicLaserEntity** current, int** boulderMap, struct game* gameData);
void updateLaserPos(struct dynamicLaserEntity** current, int*** boulderMap, struct game* gameData);
void updateAlienLaserPos(struct dynamicLaserEntity** alienCurrent, int height, int xShipOld, int yShipOld, struct game* gameData);

void detectCollisions(int xShipOld, int yShipOld, int*** boulderMap, struct game* gameData);

void generateAlien(int* alien, int* alienAI);
void updateAlienPosition(int* alien, int* alienAI, int xAlien, int yAlien, int* xAlienOld, int* yAlienOld, bool* alienState, int* alienDir, int width);

void generateBoulders(int*** boulderMap, int width, int height);
void printBoulders(int** boulderMap, int width, int height);
void updateBoulders(int*** boulderMap, int xShipOld, int yShipOld, int width, int height);

#endif