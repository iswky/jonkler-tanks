#ifndef OBSTACLE_STRUCT_H
#define OBSTACLE_STRUCT_H

#include <stdint.h>

struct RenderObject;

typedef struct {
  struct RenderObject* obstacleObject;
  uint32_t health;
} obstacleStruct;

#endif