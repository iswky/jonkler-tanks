#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "../App.h"

#define MAXSTONES 2

RenderObject* createCloud(App* app, int32_t* heightmap, int32_t startPos,
                          int32_t endPos, int32_t probability);

RenderObject* createStone(App* app, int32_t* heightmap, int32_t startPos,
                          int32_t endPos);

RenderObject* createTree(App* app, int32_t* heightmap, int32_t startPos,
                         int32_t endPos, int32_t probability);

extern uint32_t obstacleRock[MAXSTONES];
#endif