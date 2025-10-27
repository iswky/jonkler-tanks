#ifndef PERLIN_H
#define PERLIN_H

#include <stdint.h>

void generateHeightmap(int32_t* heightmap, uint32_t seed, int32_t WIDTH,
                       int32_t HEIGHT);

#endif