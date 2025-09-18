#ifndef BOT_H
#define BOT_H

#include "../App.h"

void botMain(App* app, Player* player1, Player* player2, int* heightMap,
             RenderObject* projectile, RenderObject* explosion,
             SDL_bool* regenMap, enum PlayerType playerType);

int calcBestOption(App* app, Player* firstPlayer, Player* secondPlayer,
                   int* heightMap, RenderObject* projectile,
                   RenderObject* explosion, SDL_bool* regenMap, int hitChance,
                   double initGunAngle, enum PlayerType playerType);

#endif