#include "gen_map.h"

#include "../math/perlin.h"

void addSpawnPlates(int* heightMap, int size, int dx) {
  int leftX = heightMap[dx - 1];
  int rightX = heightMap[size - 2 - dx];
  for (int i = 0; i < dx; ++i) {
    heightMap[i] = leftX;
    heightMap[size - i - 1] = rightX;
  }
}

void genHeightMap(int* heightMap, unsigned seed, int width, int height) {
  generateHeightmap(heightMap, seed, width, height);
}