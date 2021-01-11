#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#define BOULDER_DENSITY 500 //im wieksza wartosc tym mniej
#define ALIEN_CHANCE 90
#define ALIEN_DAMAGE 10
#define ALIEN_LASER_CHANCE 8

void shootLaser(struct dynamicLaserEntity** current, int xShip, int yShip);
void shootAlienLaser(struct dynamicLaserEntity** alienCurrent, struct alien alien);

void updateShipPos(struct ship* spaceship, struct dynamicLaserEntity** current, struct game* gameData);
void updateLaserPos(struct dynamicLaserEntity** current, int*** boulderMap, struct game* gameData);
void updateAlienLaserPos(struct dynamicLaserEntity** alienCurrent, int height, struct ship spaceship, struct game* gameData);

void detectCollisions(struct ship spaceship, int*** boulderMap, struct game* gameData);

void generateAlien(struct alien* alien);
void updateAlienPosition(struct alien* alien, int* alienDir, int width);

void generateBoulders(int*** boulderMap, int width, int height);
void printBoulders(int** boulderMap, int width, int height);
void updateBoulders(int*** boulderMap, int width, int height);

#endif