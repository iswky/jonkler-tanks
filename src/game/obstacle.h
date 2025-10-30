#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "../App.h"

#define MAXSTONES 2

void createClouds(App* app, RenderObject* objectsArr[],
                  SDL_bool* flag_regencloud, int32_t* count_cloud, int32_t* x,
                  int32_t* heightmap);

RenderObject* createStone(App* app, int32_t* heightmap, int32_t startPos,
                          int32_t endPos);

RenderObject* createTree(App* app, int32_t* heightmap, int32_t startPos,
                         int32_t endPos, int32_t probability);

extern uint32_t obstacleRock[MAXSTONES];
#endif