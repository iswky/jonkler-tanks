#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "../App.h"
#include "obstacle_struct.h"

#define MAXSTONES 2
#define MAXCLOUDS 5

RenderObject* createCloud(App* app, int32_t* heightmap, int32_t startPos,
                          int32_t endPos, int32_t probability);

RenderObject* createStone(App* app, int32_t* heightmap, int32_t startPos,
                          int32_t endPos);

RenderObject* createTree(App* app, int32_t* heightmap, int32_t startPos,
                         int32_t endPos, int32_t probability);

SDL_bool checkObstacleCollisions(uint32_t currX, uint32_t currY);

extern obstacleStruct obstacles[MAXSTONES + MAXCLOUDS];
#endif