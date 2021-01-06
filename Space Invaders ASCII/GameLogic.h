#ifndef GAMELOGIC_H
#define GAMELOGIC_H

void shootLaser(struct dynamicLaserEntity** current, int xShipOld, int yShipOld);
void shootAlienLaser(struct dynamicLaserEntity** alienCurrent, int xShipOld, int yShipOld);

void updateShipPos(int xShip, int yShip, int* xShipOld, int* yShipOld, struct dynamicLaserEntity** current, int** boulderMap, struct game* gameData);
void updateLaserPos(struct dynamicLaserEntity** current, int*** boulderMap, struct game* gameData);
void updateAlienLaserPos(struct dynamicLaserEntity** alienCurrent, int height);

#endif