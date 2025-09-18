#ifndef PLAYERMOVEMENT_H
#define PLAYERMOVEMENT_H

#include "../App.h"

void shoot(App* app, Player* firstPlayer, Player* secondPlayer,
           RenderObject* projectile, RenderObject* explosion, int* heightMap,
           SDL_bool* regenMap);
void recalcPlayerPos(App* app, Player* player, int* heightMap, int dx,
                     int xOffset);
void recalcPlayerGunAngle(Player* player, int dy);
int playerMove(void* data);

#endif